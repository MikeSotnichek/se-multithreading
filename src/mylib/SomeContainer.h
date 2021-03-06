#pragma once
#include <memory>
#include <map>
#include <cassert>
#include <mutex>
#include "SomeContainerIterator.h"

template<typename KeyType, typename ValueType>
using  KeyValueStore = std::map<KeyType, ValueType>;

template<typename IObject>
class CSomeContainer {
public:
    CSomeContainer();
    ~CSomeContainer();
    void Register(int objectId, std::auto_ptr<IObject> object);
    IObject* Query(int objectId);
    void Unregister(int objectId);
    CSomeContainerIterator<IObject> Start();
    CSomeContainerIterator<IObject> End();
private:
    void ImplUnregister(int objectId);
private:
    KeyValueStore<int, IObject*> m_storage;
    std::mutex m_mutex;
};

template<typename IObject>
CSomeContainer<IObject>::CSomeContainer()
{
}

template<typename IObject>
CSomeContainer<IObject>::~CSomeContainer()
{
    try
    {
        for (auto it = m_storage.begin(); it != m_storage.end(); ++it)
        {
            if (it->second != nullptr) {
                delete it->second;
            }
        }
    } catch (const std::exception &) {
        //
    }
}

template<typename IObject>
void CSomeContainer<IObject>::Register(int objectId, std::auto_ptr<IObject> object)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_storage[objectId] != nullptr) {
        ImplUnregister(objectId);
    }
    m_storage[objectId] = object.release();
}

template<typename IObject>
IObject* CSomeContainer<IObject>::Query(int objectId)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_storage.at(objectId);
}

template<typename IObject>
void CSomeContainer<IObject>::Unregister(int objectId)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    ImplUnregister(objectId);
}

template<typename IObject>
CSomeContainerIterator<IObject> CSomeContainer<IObject>::Start()
{
    return CSomeContainerIterator<IObject>(this, m_storage.begin());
}

template<typename IObject>
CSomeContainerIterator<IObject> CSomeContainer<IObject>::End()
{
    return CSomeContainerIterator<IObject>(this, m_storage.end());
}

template<typename IObject>
void CSomeContainer<IObject>::ImplUnregister(int objectId)
{
    try {
        IObject* objPtr = m_storage.at(objectId);
        delete objPtr;
        m_storage.erase(objectId);
    } catch (const std::out_of_range&) {

    }
}
