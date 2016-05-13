
#include <string>
#include <vector>

class Compiler {
    enum Type {
        Float1, Float2
    };

    struct Chunk {
        Type type;
        std::string symbol;
        std::string code;
        bool isInline;
    };

    std::string code;
    std::vector<Chunk> chunks;

    int addInlineChunk(Type type, const char* fmt, ...) {
        char text[1024];

        va_list list;
        va_start(list, fmt);
        vsnprintf(text, 1024, fmt, list);
        va_end(list);

        int index = chunks.size();

        Chunk chunk;
        chunk.type = type;
        chunk.code = text;
        chunk.isInline = true;
        chunks.push_back(chunk);

        return index;
    }

    int addChunk(Type type, bool x, const char* fmt, ...) {
        char text[1024];
        char symbol[100];

        va_list list;
        va_start(list, fmt);
        vsnprintf(text, 1024, fmt, list);
        va_end(list);

        int index = chunks.size();

        snprintf(symbol, sizeof(symbol), "local%d", index);

        Chunk chunk;
        chunk.type = type;
        chunk.symbol = symbol;
        chunk.code = (x ? (std::string("vec2 ") + symbol + " = ") : "") + text + ";\n";
        chunk.isInline = false;
        chunks.push_back(chunk);

        return index;
    }
public:

    void func(const char* name) {
        addChunk(Type::Float1, false, "%s()", name);
    }

    int cos(int index) {
        Type type = chunks[index].type;
        return addChunk(type, true, "cos(%s)", chunks[index].code.c_str());
    }

    int sin(int index) {
        Type type = chunks[index].type;
        return addChunk(type, true, "sin(%s)", chunks[index].code.c_str());
    }

    int max(int index0, int index1) {
        Type type = chunks[index0].type;
        return addChunk(type, true, "max(%s, %s)", chunks[index0].symbol.c_str(), chunks[index1].symbol.c_str());
    }

    int add(int index0, int index1) {
        Type type = chunks[index0].type;
        return addChunk(type, true, "(%s + %s)", chunks[index0].symbol.c_str(), chunks[index1].symbol.c_str());
    }

    int mult(int index0, int index1) {
        Type type = chunks[index0].type;
        return addChunk(type, true, "(%s * %s)", chunks[index0].symbol.c_str(), chunks[index1].symbol.c_str());
    }

    int position() {
        return addInlineChunk(Type::Float2, "in_Position");
    }

    int texture() {
        return addInlineChunk(Type::Float2, "in_Texture");
    }

    int constant(float value) {
        return addChunk(Type::Float2, true, "%f", value);
    }

    int ret(int index) {
        return addChunk(Type::Float2, false, "return %s", chunks[index].symbol.c_str());
    }

    void print() {
        for(int i = 0; i < chunks.size(); i++) {
            if(!chunks[i].isInline)
                printf("%s", chunks[i].code.c_str());
        }
    }
};

void testCompiler() {
    Compiler compiler;

    compiler.func("main");

    compiler.ret(
            compiler.add(
                    compiler.constant(10),
                    compiler.max(
                            compiler.mult(compiler.cos(compiler.position()), compiler.constant(20)),
                            compiler.sin(compiler.texture())
                    ))
    );

    compiler.print();
}