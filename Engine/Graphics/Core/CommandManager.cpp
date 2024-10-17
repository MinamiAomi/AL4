#include "CommandManager.h"

CommandManager::CommandManager() :
    commandQueue_(D3D12_COMMAND_LIST_TYPE_DIRECT), 
    commandAllocatorPool_(D3D12_COMMAND_LIST_TYPE_DIRECT), 
    commandListPool_(D3D12_COMMAND_LIST_TYPE_DIRECT) {
}

void CommandManager::Create() {
    commandQueue_.Create();
}

UINT64 CommandManager::Add(Microsoft::WRL::ComPtr<ID3D12CommandList> commandList) {
    std::lock_guard<std::mutex> lock(mutex_);
    closedCommandLists_.emplace_back(commandList);

    return commandQueue_.GetNextFenceValue();
}

void CommandManager::Execute() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ID3D12CommandList*> pplist(closedCommandLists_.size());
    for (size_t i = 0; i < pplist.size(); ++i) {
        pplist[i] = closedCommandLists_[i].Get();
    }
    commandQueue_.ExecuteCommandLists(pplist.data(), (UINT)pplist.size());
    closedCommandLists_.clear();
}
