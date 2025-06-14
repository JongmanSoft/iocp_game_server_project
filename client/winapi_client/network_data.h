#pragma once
#include "stdafx.h"

class NonBlockingClient {
private:
    NonBlockingClient();
    ~NonBlockingClient();
public:
    void init(const std::string& server_ip, short port);
    
    static NonBlockingClient& get_inst() {
        static NonBlockingClient inst;
        return inst;
    }

    bool connectToServer();
    void disconnect();

    // ��Ŷ ���� �Լ���
    void sendLoginPacket(const std::string& name);
    void sendMovePacket(char direction);
    void sendAttackPacket();
    void sendChatPacket(const std::string& message);
    void sendTeleportPacket();

    // ��Ŷ ���� �ݹ� ����
    using PacketHandler = std::function<void(const char*, size_t)>;
    void setPacketHandler(PacketHandler handler) { packetHandler_ = handler; }

public:
    void processNetwork();
    void handleReceivedPacket(const char* buffer, size_t size);
    void sendPendingPackets();

    SOCKET socket_;
    std::string server_ip_;
    short port_;
    std::atomic<bool> running_;
    std::thread networkThread_;
    std::mutex sendMutex_;
    std::queue<std::vector<char>> sendQueue_;
    PacketHandler packetHandler_;
    std::vector<char> recvBuffer_; // ���� ���۷� ����
    size_t bufferOffset_ = 0;

    fd_set readSet, writeSet;
    timeval timeout = { 0, 10000 }; // 10ms Ÿ�Ӿƿ�

};