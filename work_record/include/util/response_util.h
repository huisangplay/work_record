#pragma once

#include <httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace httplib;
using json = nlohmann::json;

namespace response_util {

    /**
     * 发送成功响应
     * @param res HTTP响应对象
     * @param data 响应数据
     * @param message 成功消息（可选）
     */
    inline void send_success(Response& res, const json& data = json{}, const std::string& message = "") {
        json response = {
            {"success", true},
            {"data", data}
        };
        if (!message.empty()) {
            response["message"] = message;
        }
        res.set_content(response.dump(), "application/json");
    }

    /**
     * 发送错误响应
     * @param res HTTP响应对象
     * @param message 错误消息
     * @param status_code HTTP状态码（默认500）
     */
    inline void send_error(Response& res, const std::string& message, int status_code = 500) {
        res.status = status_code;
        res.set_content(json{
            {"error", false},
            {"message", message}
        }.dump(), "application/json");
    }

    /**
     * 发送服务器内部错误响应
     * @param res HTTP响应对象
     */
    inline void send_internal_error(Response& res) {
        send_error(res, "服务器内部错误", 500);
    }

    /**
     * 发送参数错误响应
     * @param res HTTP响应对象
     * @param message 错误消息
     */
    inline void send_bad_request(Response& res, const std::string& message) {
        send_error(res, message, 400);
    }

    /**
     * 发送未找到错误响应
     * @param res HTTP响应对象
     * @param message 错误消息
     */
    inline void send_not_found(Response& res, const std::string& message) {
        send_error(res, message, 404);
    }

    /**
     * 发送操作失败响应
     * @param res HTTP响应对象
     * @param operation 操作名称（如"插入"、"更新"、"删除"等）
     * @param entity 实体名称（如"员工"、"部门"、"工单"等）
     */
    inline void send_operation_failed(Response& res, const std::string& operation, const std::string& entity) {
        send_error(res, entity + operation + "失败");
    }

    /**
     * 直接发送数据响应（不包装在data字段中）
     * 用于字典接口等需要直接返回数组的场景
     * @param res HTTP响应对象
     * @param data 响应数据
     */
    inline void send_data_direct(Response& res, const json& data) {
        res.set_content(data.dump(), "application/json");
    }

} // namespace response_util 