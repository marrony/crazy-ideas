//
// Created by Marrony Neris on 11/18/15.
//

#ifndef STREAM_H
#define STREAM_H

class Stream {
    virtual void serialize(int& value) = 0;
public:
    friend Stream& operator<<(Stream& stream, int& value) {
        stream.serialize(value);
        return stream;
    }
};

class ReadStream : public Stream {
    virtual void serialize(int& value) {
    }
public:
};

class WriteStream : public Stream {
    virtual void serialize(int& value) {
    }
public:
};

class Object {
    int field0;
    char field1;
public:
    void serialize(Stream& stream) {
        stream << field0;
        stream << field1;
    }
};

#endif //STREAM_H
