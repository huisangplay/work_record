// 公共API封装，可根据需要扩展
const api = {
  get: (url, params) => axios.get(url, { params }).catch(err => { handleApiError(err, '请求失败'); throw err; }),
  post: (url, data) => axios.post(url, data).catch(err => { handleApiError(err, '请求失败'); throw err; }),
  put: (url, data) => axios.put(url, data).catch(err => { handleApiError(err, '请求失败'); throw err; }),
  delete: (url, params) => axios.delete(url, { params }).catch(err => { handleApiError(err, '请求失败'); throw err; })
}; 