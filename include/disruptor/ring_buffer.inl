template<class T>
inline ring_buffer<T>::ring_buffer(size_t n) :
    _buffer(new T[n]),
    _size(n),
    _mask(n - 1) {
}

template<class T>
inline ring_buffer<T>::~ring_buffer() {
    delete[] _buffer;
}

template<class T>
inline size_t ring_buffer<T>::size() {
    return _size;
}

template<class T>
inline T& ring_buffer<T>::operator[](int i) {
    return _buffer[i&_mask];
}
