inline sequencer::sequencer(sequence& p, sequence& s, size_t n) :
    _pub_seq(p),
    _sub_seq(s),
    _size(n),
    _curr(-1),
    _cache(-1) {
}

inline sequence::value_type sequencer::next(size_t n) {
    long next = _curr + 1;
    long wrap = next - _size;
    if (wrap > _cache || _cache > next) {
        while (wrap > (_cache = _sub_seq.get())) {
            std::this_thread::yield();
        }
    }
    _curr = next;
    return _curr;
}

inline void sequencer::publish() {
    _pub_seq.set(_curr);
}
