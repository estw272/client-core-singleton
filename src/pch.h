#pragma once
#ifndef _PRECOMP_H_2389234
#define _PRECOMP_H_2389234

#define BOOST_ASIO_DISABLE_CONCEPTS

// std
#include <iostream>
#include <filesystem>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <deque>
#include <functional>
#include <sstream>
#include <fstream>
#include <mutex>
#include <set>
#include <algorithm>
#include <optional>
#include <atomic>

// diff
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "boost/asio.hpp"
#include "boost/bind.hpp"

//winapi
#include <WS2tcpip.h>
#include <windows.h>

// my
#include "macros.h"

#endif