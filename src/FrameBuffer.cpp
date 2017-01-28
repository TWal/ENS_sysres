#include "FrameBuffer.h"

static char* const FB = (char*)0xB8000;

FrameBuffer::FrameBuffer() {
    clear();
}

void FrameBuffer::clear() {
    _fg = 0xF;
    _bg = 0x0;
    _cursCol = 0;
    _cursRow = 0;
    for(int i = 0 ; i < 80 * 25 ; ++i){
        FB[2*i] = 0;
        FB[2*i+1] = 0xF;
    }
}

void FrameBuffer::moveCursor(int col, int row, bool updateCurs) {
    _cursCol = col;
    _cursRow = row;
    if(updateCurs) updateCursor();
}

void FrameBuffer::updateCursor() {
    static const ushort CURSOR_COMMAND_PORT = 0x3D4;
    static const ushort CURSOR_DATA_PORT = 0x3D5;
    static const ushort CURSOR_HIGH_BYTE = 0xE;
    static const ushort CURSOR_LOW_BYTE = 0xF;

    int pos = 80*_cursRow + _cursCol;
    outb(CURSOR_COMMAND_PORT, CURSOR_HIGH_BYTE);
    outb(CURSOR_DATA_PORT, (pos>>8) & 0xFF);
    outb(CURSOR_COMMAND_PORT, CURSOR_LOW_BYTE);
    outb(CURSOR_DATA_PORT, pos & 0xFF);
}

void FrameBuffer::writeChar(char c, int col, int row, char fg, char bg) {
    int i = 80*row + col;
    FB[2*i] = c;
    FB[2*i+1] = (fg & 0xF) | ((bg & 0xF) << 4);
}

void FrameBuffer::scroll(uint n, bool updateCurs) {
    for(int row = 0; row < 25; ++row) {
        for(int col = 0; col < 80; ++col) {
            int posTo = 80*row+col;
            if(row+n < 25) {
                int posFrom = 80*(row+n)+col;
                FB[2*posTo] = FB[2*posFrom];
                FB[2*posTo+1] = FB[2*posFrom+1];
            } else {
                FB[2*posTo] = 0;
                FB[2*posTo+1] = 0xF;
            }
        }
    }

    _cursRow = max(_cursRow - (int)n, 0);
    if(updateCurs) updateCursor();
}

void FrameBuffer::putc(char c, bool updateCurs) {
    switch(c) {
        case '\n':
            _cursCol = 0;
            _cursRow++;
            break;
        case '\r':
            _cursCol = 0;
            break;
        case '\f':
            _cursRow++;
            break;
        case '\t':
            _cursCol += 4-(_cursCol%4);
            break;
        case '\b':
            _cursCol = max(0, _cursCol-1);
            break;
        default:
            writeChar(c, _cursCol, _cursRow, _fg, _bg);
            _cursCol++;
            break;
    }
    if(_cursCol >= 80) {
        _cursCol -= 80;
        _cursRow += 1;
    }
    if(_cursRow >= 25) {
        scroll(1, false);
    }
    if(updateCurs) updateCursor();
}

void FrameBuffer::puts(const char* s, bool updateCurs) {
    for(; *s; ++s) {
        putc(*s, false);
    }
    if(updateCurs) updateCursor();
}
