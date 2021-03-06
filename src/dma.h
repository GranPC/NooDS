/*
    Copyright 2019-2020 Hydr8gon

    This file is part of NooDS.

    NooDS is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NooDS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NooDS. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DMA_H
#define DMA_H

#include <cstdint>

class Cartridge;
class Gpu3D;
class Interpreter;
class Memory;

class Dma
{
    public:
        Dma(Cartridge *cart, Gpu3D *gpu3D, Interpreter *cpu, Memory *memory):
            cart(cart), gpu3D(gpu3D), cpu(cpu), memory(memory) {}

        void transfer();

        bool shouldTransfer() { return enabled; }

        uint32_t readDmaSad(int channel) { return dmaSad[channel]; }
        uint32_t readDmaDad(int channel) { return dmaDad[channel]; }
        uint32_t readDmaCnt(int channel) { return dmaCnt[channel]; }

        void writeDmaSad(int channel, uint32_t mask, uint32_t value);
        void writeDmaDad(int channel, uint32_t mask, uint32_t value);
        void writeDmaCnt(int channel, uint32_t mask, uint32_t value);

    private:
        uint32_t dmaSad[4] = {};
        uint32_t dmaDad[4] = {};
        uint32_t dmaCnt[4] = {};

        uint32_t srcAddrs[4] = {};
        uint32_t dstAddrs[4] = {};
        uint32_t wordCounts[4] = {};
        uint8_t enabled = 0;

        int gxFifoCount = 0;

        Cartridge *cart;
        Gpu3D *gpu3D;
        Interpreter *cpu;
        Memory *memory;
};

#endif // DMA_H
