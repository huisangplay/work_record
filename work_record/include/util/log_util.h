#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <vector>
#include <memory>
#include <exception>
#include <iostream>



namespace log_util {
    // 打印异常信息的辅助函数
    static void log_exception(const std::exception& e, const std::string& context = "") {
        std::string prefix = context.empty() ? "" : context + ": ";
        
        // 输出异常信息
        spdlog::error("{}Exception: {}", prefix, e.what());
        
    }

    // 通用异常处理宏
    #define LOG_EXCEPTION(e, context) log_util::log_exception(e, context)
    #define LOG_EXCEPTION_AND_RETHROW(e, context) \
        do { \
            log_util::log_exception(e, context); \
            throw; \
        } while(0)

    static void init(const std::string &logger_name, const std::string &log_file,
                     spdlog::level::level_enum console_level = spdlog::level::debug,
                     spdlog::level::level_enum file_level = spdlog::level::info) {
        try {
            // 初始化异步日志线程池（如果还没有初始化）
            if (!spdlog::thread_pool()) {
                spdlog::init_thread_pool(8192, 1);
            }

            // 创建控制台 sink（带颜色）
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
            console_sink->set_level(console_level);

            // 创建文件 sink
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
            file_sink->set_level(file_level);

            // 将两个 sink 组合
            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

            // 创建异步日志器
            auto async_logger = std::make_shared<spdlog::async_logger>(logger_name, sinks.begin(), sinks.end(),
                                                                        spdlog::thread_pool(),
                                                                        spdlog::async_overflow_policy::block);
            // 设置日志级别为两个级别中的较低值，确保所有消息都能被处理
            auto global_level = (console_level < file_level) ? console_level : file_level;
            async_logger->set_level(global_level);
            async_logger->flush_on(global_level);

            // 设置为全局默认日志器
            spdlog::set_default_logger(async_logger);

            spdlog::info("Logger initialized successfully. Console level: {}, File level: {}",
                        spdlog::level::to_string_view(console_level),
                        spdlog::level::to_string_view(file_level));
        } catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    // 清理日志系统
    static void shutdown() {
        try {
            // 获取默认日志器并刷新
            auto logger = spdlog::get("app_logger");
            if (logger) {
                logger->flush();
            }
            spdlog::shutdown();
        } catch (const std::exception& e) {
            std::cerr << "Log shutdown failed: " << e.what() << std::endl;
        }
    }
}
