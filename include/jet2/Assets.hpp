#pragma once
namespace jet2 {
struct Asset {
    char const* name;
    char const* data;
    size_t len;
};
extern Asset const assets[];
}
