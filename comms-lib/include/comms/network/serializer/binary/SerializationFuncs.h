#ifndef CAMB_NETWORK_BINARY_SERIALIZATIONFUNCS_H_
#define CAMB_NETWORK_BINARY_SERIALIZATIONFUNCS_H_

#include <cstddef>
#include <array>
#include "comms/network/Buffer.h"

namespace cadf::comms::binary {

    template<typename T>
    struct DataSerializer;

    /**
     * Helper function to determine the size of the data. A default implementation is provided to perform "sizeof",
     * but a specialized implementation is to be provided for any/all data structures for which a simple "sizeof"
     * is insufficient.
     *
     * @template T typename indicating the type of data whose size is wanted
     *
     * @param &data const reference to the data whose size is to be determined
     *
     * @return the size of the data as size_t
     */
    template<typename T>
    size_t sizeOfData(const T &data) {
        return sizeof(T);
    }

    template<>
    size_t sizeOfData<std::string>(const std::string &data);

    /**
     * Helper function to determine the size of the data pointed to. A default implementation is provided to perform "sizeof",
     * but a specialized implementation is to be provided for any/all data structures for which a simple "sizeof"
     * is insufficient.
     *
     * @template T typename indicating the type of data that the pointer is pointing to
     *
     * @param *data const T pointer to the data
     *
     * @return the size of the data as size_t
     */
    template<typename T>
    size_t sizeOfPointer(const T *data) {
        return sizeof(T);
    }

    /**
     * Helper function to determine the size of the data within an array.
     *
     * @template T typename indicating the type of data that is contained within the array
     * @template numOfElements size_t the number of elements that are present in the array
     *
     * @param &data const std::array reference to the array data
     *
     * @return the size of the data as size_t
     */
    template<typename T, size_t numOfElements>
    size_t sizeOfArray(const std::array<T, numOfElements> &data) {
        size_t size = 0;
        for (size_t i = 0; i < numOfElements; i++)
            size += DataSerializer<T>::sizeOf(data[i]);

        return size;
    }

    /**
     * Helper function to determine the size of the data contained within a dynamic array.
     *
     * @template V the class representing the dynamic array
     * @template T the type that is stored within the array
     * @template Alloc the allocator used with the dynamic array
     *
     * @return the size of the data as size_t
     */
    template<template<typename, typename > class V, typename T, typename Alloc>
    size_t sizeOfDynamicArray(const V<T, Alloc> &data) {
        size_t size = sizeof(size_t);
        for (const T &t : data)
            size += DataSerializer<T>::sizeOf(t);

        return size;
    }

    /**
     * Helper function to determine the size of the data contained within a map.
     *
     * @template M the class representing the map
     * @template K the type used for the map key
     * @template T the type that is stored within the map
     * @template Comp the comparison operator for the key
     * @template Alloc the allocator used with the map
     *
     * @return the size of the data as size_t
     */
    template<template<typename, typename, typename, typename > class M, typename K, typename T, typename Comp, typename Alloc>
    size_t sizeOfMap(const M<K, T, Comp, Alloc> &data) {
        size_t size = sizeof(size_t);
        for (std::pair<const K&, const T&> element : data)
            size += DataSerializer<K>::sizeOf(element.first) + DataSerializer<T>::sizeOf(element.second);

        return size;
    }

    /**
     * Performs the serialization of a scalar, ensuring that the data referenced is properly copied into the provided buffer.
     *
     * @template T typename indicating the type of pointer to be serialized
     *
     * @param &data const T the data to be serialized
     * @param *buffer OutputBuffer pointer where the data is to be copied to
     */
    template<typename T>
    void serializeData(const T &data, OutputBuffer *buffer) {
        buffer->append(data, sizeOfData(data));
    }

    template<>
    void serializeData<std::string>(const std::string &data, OutputBuffer *buffer);

    /**
     * Performs the serialization of a pointer, ensuring that the data pointed to properly copied into the provided buffer.
     *
     * @template T typename indicating the type of data to be serialized
     *
     * @param *ptr const T pointer to the data to be serialized
     * @param *buffer OutputBuffer pointer where the data is to be copied to
     */
    template<typename T>
    void serializePointer(const T *ptr, OutputBuffer *buffer) {
        buffer->append(*ptr, sizeof(T));
    }

    /**
     * Performs the serialization of an array.
     *
     * @template T typename indicating the type of data within the array
     * @template numOfElements size_t the number of elements that are present in the array
     *
     * @param &data const std::array reference to the data array
     * @param *buffer OutputBuffer pointer where the data is to be copied to
     */
    template<typename T, size_t numOfElements>
    void serializeArray(const std::array<T, numOfElements> &data, OutputBuffer *buffer) {
        for (size_t i = 0; i < numOfElements; i++)
            DataSerializer<T>::serialize(data[i], buffer);
    }

    /**
     * Performs the serialization of a dynamic array.
     *
     * @template V the class representing the dynamic array
     * @template T the type that is stored within the array
     * @template Alloc the allocator used with the dynamic array
     *
     * @param &data const reference to the dynamic array of data
     * @param *buffer OutputBuffer pointer where the data is to be copied to
     */
    template<template<typename, typename > class V, typename T, typename Alloc>
    void serializeDynamicArray(const V<T, Alloc> &data, OutputBuffer *buffer) {
        serializeData(data.size(), buffer);
        for (const T &t : data)
            DataSerializer<T>::serialize(t, buffer);
    }

    /**
     * Performs the serialization of a map.
     *
     * @template M the class representing the map
     * @template K the type used for the map key
     * @template T the type that is stored within the map
     * @template Comp the comparison operator for the key
     * @template Alloc the allocator used with the map
     *
     * @param &data const reference to the map of data
     * @param *buffer OutputBuffer pointer where the data is to be copied to
     */
    template<template<typename, typename, typename, typename > class M, typename K, typename T, typename Comp, typename Alloc>
    void serializeMap(const M<K, T, Comp, Alloc> &data, OutputBuffer *buffer) {
        serializeData(data.size(), buffer);
        for (std::pair<const K&, const T&> element : data) {
            DataSerializer<K>::serialize(element.first, buffer);
            DataSerializer<T>::serialize(element.second, buffer);
        }
    }

    /**
     * Performs the deserialization of the data, ensuring that it is properly copied out of the buffer.
     *
     * @template T typename indicating the data of pointer to be deserialized
     *
     * @param *buffer InputBuffer pointer where the data is to be copied from
     *
     * @return T the data copied from the buffer
     */
    template<typename T>
    T deserializeData(InputBuffer *buffer) {
        return buffer->retrieveNext<T>(sizeof(T));
    }

    template<>
    std::string deserializeData<std::string>(InputBuffer *buffer);

    /**
     * Performs the deserialization of a pointer, ensuring that the data in the buffer is properly copied out of the buffer.
     *
     * @template T typename indicating the type of pointer to be deserialized
     *
     * @param *buffer InputBuffer pointer where the data is to be copied from
     *
     * @return pointer to where the data from the buffer has been copied to
     */
    template<class T>
    T* deserializePointer(InputBuffer *buffer) {
        return buffer->retrieveNext<T*>(sizeof(T));
    }

    /**
     * Performs the deserialization of an array.
     *
     * @template T typename indicating the type of data within the array
     * @template numOfElements size_t the number of elements that are present in the array
     *
     * @param *buffer InputBuffer pointer where the data is to be copied from
     *
     * @return std::array with the buffered elements
     */
    template<typename T, size_t numOfElements>
    std::array<T, numOfElements> deserializeArray(InputBuffer *buffer) {
        std::array<T, numOfElements> data;
        for (size_t i = 0; i < numOfElements; i++)
            data[i] = DataSerializer<T>::deserialize(buffer);

        return data;
    }

    /**
     * Performs the deserialization of a dynamic array.
     *
     * @template V the class representing the dynamic array
     * @template T the type that is stored within the array
     * @template Alloc the allocator used with the dynamic array
     *
     * @param *buffer InputBuffer pointer where the data is to be copied from
     *
     * @return dynamic array as populated from the buffer
     */
    template<template<typename, typename > class V, typename T, typename Alloc>
    V<T, Alloc> deserializeDynamicArray(InputBuffer *buffer) {
        size_t numOfElements = deserializeData<size_t>(buffer);
        V<T, Alloc> data(numOfElements);
        for (size_t i = 0; i < numOfElements; i++)
            data[i] = DataSerializer<T>::deserialize(buffer);

        return data;
    }

    /**
     * Performs the deserialization of a map.
     *
     * @template M the class representing the map
     * @template K the type used for the map key
     * @template T the type that is stored within the map
     * @template Comp the comparison operator for the key
     * @template Alloc the allocator used with the map
     *
     * @param *buffer InputBuffer pointer where the data is to be copied from
     *
     * @return map as populated from the buffer
     */
    template<template<typename, typename, typename, typename > class M, typename K, typename T, typename Comp, typename Alloc>
    const M<K, T, Comp, Alloc> deserializeMap(InputBuffer *buffer) {
        size_t numOfElements = deserializeData<size_t>(buffer);
        M<K, T, Comp, Alloc> data;
        for (size_t i = 0; i < numOfElements; i++) {
            K key = DataSerializer<K>::deserialize(buffer);
            T val = DataSerializer<T>::deserialize(buffer);
            data[key] = val;
        }
        return data;
    }
}

#endif /* CAMB_NETWORK_BINARY_SERIALIZATIONFUNCS_H_ */
