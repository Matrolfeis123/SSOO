#pragma once

struct proceso{
    char nombre;
    int pid;
    char estado;
    int burst;
    int io_waiting;
};