#include "Settings.h"

Settings& Settings::instance() {
    static Settings s;
    return s;
}
