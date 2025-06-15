#include "network_data.h"

NonBlockingClient::~NonBlockingClient()
{
	disconnect();
}

void NonBlockingClient::frame_work_recv(Framework* fw)
{
    m_framwork = fw;
}

void NonBlockingClient::init(const std::string& server_ip, short port)
{
	server_ip_ = server_ip;
	port_ = port;
	socket_ = INVALID_SOCKET;
	running_ = false;

    recvBuffer_.resize(BUF_SIZE * 2); 
}

bool NonBlockingClient::connectToServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    // 논블로킹 모드 설정
    u_long mode = 1;
    ioctlsocket(socket_, FIONBIO, &mode);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    inet_pton(AF_INET, server_ip_.c_str(), &serverAddr.sin_addr);

    if (connect(socket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            closesocket(socket_);
            WSACleanup();
            return false;
        }
    }

    running_ = true;
    return true;
}

void NonBlockingClient::disconnect()
{
    running_ = false;
    if (networkThread_.joinable()) {
        networkThread_.join();
    }
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
    WSACleanup();
}

void NonBlockingClient::sendLoginPacket(const std::string& name)
{
    cs_packet_login packet = {};
    packet.size = sizeof(cs_packet_login);
    packet.type = C2S_P_LOGIN;
    strncpy_s(packet.name, name.c_str(), MAX_ID_LENGTH - 1);

    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}

void NonBlockingClient::sendMovePacket(char direction)
{
    cs_packet_move packet = {};
    packet.size = sizeof(cs_packet_move);
    packet.type = C2S_P_MOVE;
    packet.direction = direction;

    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}

void NonBlockingClient::sendAttackPacket(char direction)
{
    cs_packet_attack packet = {};
    packet.size = sizeof(cs_packet_attack);
    packet.type = C2S_P_ATTACK;
    packet.direction = direction;

    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}


void NonBlockingClient::sendChatPacket(const char* message)
{
    cs_packet_chat packet = {};
    packet.size = sizeof(cs_packet_chat);
    packet.type = C2S_P_CHAT;
    for (int i = 0; i < MAX_CHAT_LENGTH; i++)packet.message[i] = '\0';
    strcpy_s(packet.message, message);
    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}

void NonBlockingClient::sendTeleportPacket()
{
    cs_packet_teleport packet = {};
    packet.size = sizeof(cs_packet_teleport);
    packet.type = C2S_P_TELEPORT;

    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}

void NonBlockingClient::sendStatePacket(int state, char dir)
{
    cs_packet_state packet = {};
    packet.size = sizeof(cs_packet_state);
    packet.type = C2S_P_STATE;
    packet.direction = dir;
    packet.state = state;
    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace(std::vector<char>(reinterpret_cast<char*>(&packet), reinterpret_cast<char*>(&packet) + packet.size));
}

void NonBlockingClient::processNetwork()
{

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_SET(socket_, &readSet);
    if (!sendQueue_.empty()) {
        FD_SET(socket_, &writeSet);
    }

    int result = select(0, &readSet, &writeSet, nullptr, &timeout);
    if (result == SOCKET_ERROR) {
        disconnect();
        return;
    }

    if (FD_ISSET(socket_, &readSet)) {
        // 버퍼에 여유 공간이 있는지 확인
        if (bufferOffset_ >= recvBuffer_.size()) {
            recvBuffer_.resize(recvBuffer_.size() * 2); // 필요 시 버퍼 확장
        }

        int bytesReceived = recv(socket_, recvBuffer_.data() + bufferOffset_,
            recvBuffer_.size() - bufferOffset_, 0);
        if (bytesReceived <= 0) {
            if (bytesReceived == 0 || WSAGetLastError() != WSAEWOULDBLOCK) {
                disconnect();
                return;
            }
        }
        else {
            bufferOffset_ += bytesReceived;
            // 수신된 데이터를 패킷 단위로 처리
            size_t processed = 0;
            while (processed < bufferOffset_) {
                if (bufferOffset_ - processed < sizeof(unsigned char)) {
                    break; // size 필드조차 읽을 수 없음
                }

                unsigned char packetSize = static_cast<unsigned char>(recvBuffer_[processed]);
                if (packetSize < 2 || packetSize > MAX_CHAT_LENGTH + 10) {
                    // 비정상적인 패킷 크기, 에러 처리
                    disconnect();
                    break;
                }

                if (processed + packetSize > bufferOffset_) {
                    break; // 완전한 패킷이 아직 도착하지 않음
                }

                // 완전한 패킷 처리
                handleReceivedPacket(recvBuffer_.data() + processed, packetSize);
                processed += packetSize;
            }

            // 처리된 데이터 제거
            if (processed > 0) {
                std::move(recvBuffer_.begin() + processed, recvBuffer_.begin() + bufferOffset_,
                    recvBuffer_.begin());
                bufferOffset_ -= processed;
            }
        }
    }

    if (FD_ISSET(socket_, &writeSet)) {
        sendPendingPackets();
    }

}

void NonBlockingClient::handleReceivedPacket(const char* buffer, size_t size)
{
    if (size < 2) return;
    char packetType = buffer[1];
    switch (packetType) {
    case S2C_P_AVATAR_INFO: {
     
        if (size < sizeof(sc_packet_avatar_info)) return;
        const sc_packet_avatar_info* packet = reinterpret_cast<const sc_packet_avatar_info*>(buffer);
        strcpy_s(m_framwork->player_login_info.name, packet->name);
        m_framwork->player_login_info.x = packet->x;
        m_framwork->player_login_info.y = packet->y;
        m_framwork->player_login_info.level = packet->level;
        m_framwork->player_login_info.hp = packet->hp;
        m_framwork->player_login_info.id = packet->id;
 
        m_framwork->scene_change(PLAY_SCENE);

        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_CHAT: {
        if (size < sizeof(sc_packet_chat)) return;
        const sc_packet_chat* packet = reinterpret_cast<const sc_packet_chat*>(buffer);
        m_framwork->chat_packet_process(*packet);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_ENTER: {
        if (size < sizeof(sc_packet_enter)) return;
        const sc_packet_enter* packet = reinterpret_cast<const sc_packet_enter*>(buffer);
        m_framwork->add_packet_process(*packet);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_LEAVE: {
        if (size < sizeof(sc_packet_leave)) return;
        const sc_packet_leave* packet = reinterpret_cast<const sc_packet_leave*>(buffer);
        m_framwork->leave_packet_process(*packet);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_LOGIN_FAIL: {
        if (size < sizeof(sc_packet_login_fail)) return;
        const sc_packet_login_fail* packet = reinterpret_cast<const sc_packet_login_fail*>(buffer);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_MOVE:{
        if (size < sizeof(sc_packet_move)) return;
        const sc_packet_move* packet = reinterpret_cast<const sc_packet_move*>(buffer);
        m_framwork->move_packet_process(*packet);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_STAT_CHANGE: {
        if (size < sizeof(sc_packet_stat_change)) return;
        const sc_packet_stat_change* packet = reinterpret_cast<const sc_packet_stat_change*>(buffer);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    case S2C_P_STATE: {
        if (size < sizeof(sc_packet_state)) return;
        const sc_packet_state* packet = reinterpret_cast<const sc_packet_state*>(buffer);
        m_framwork->state_packet_process(*packet);
        if (packetHandler_) {
            packetHandler_(buffer, size);
        }
        break;
    }
    default:
 
        break;
    }
}

void NonBlockingClient::sendPendingPackets()
{
    std::lock_guard<std::mutex> lock(sendMutex_);
    while (!sendQueue_.empty()) {
        auto& packet = sendQueue_.front();
        int sent = send(socket_, packet.data(), packet.size(), 0);
        if (sent == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                disconnect();
                break;
            }
            break;
        }
        sendQueue_.pop();
    }
}




