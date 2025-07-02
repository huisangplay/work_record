// 通用工具函数
function showSwalToast(type, title, timer = 1500) {
  if (typeof Swal !== 'undefined') {
    Swal.fire({
      position: "bottom-end",
      icon: type, // 'success' | 'error' | 'info' | 'warning'
      title: title,
      showConfirmButton: false,
      timer: timer,
      customClass: {
        popup: 'swal-toast-sm',
        title: 'swal-toast-title-sm'
      },
      toast: true
    });
  } else {
    alert(title);
  }
}

// 统一 catch 错误处理
function handleApiError(err, fallbackMsg = '操作失败') {
  let msg = fallbackMsg;
  if (err?.response?.data?.error) msg = err.response.data.error;
  else if (err?.message) msg = err.message;
  showSwalToast('error', msg, 2000);
}

// 通用下拉字典加载
function loadDictToSelect(table, $select, placeholder = '请选择') {
  return axios.get('/api/dict', {params:{table}}).then(res=>{
    let html = `<option value="">${placeholder}</option>`;
    for(const d of res.data) {
      let text = d.type || d.name || d.status || d.progress || d.affected || d.comment || '';
      html += `<option value="${d.id}">${text}</option>`;
    }
    $select.html(html);
  }).catch(err => handleApiError(err, '加载字典失败'));
}

// 加载部门下拉
function loadDepartmentOptions($select, selectedId, cb) {
  return axios.get('/api/departments').then(res => {
    let html = '<option value="">请选择</option>';
    for(const d of res.data) {
      html += `<option value="${d.id}">${d.name}</option>`;
    }
    $select.html(html);
    if (selectedId) $select.val(selectedId);
    if (cb) cb();
  }).catch(err => handleApiError(err, '加载部门失败'));
}

// 加载员工下拉（按部门）
function loadEmployeeOptionsByDept($select, departmentId, selectedId) {
  if (!departmentId) {
    $select.html('<option value="">请先选择部门</option>');
    return Promise.resolve();
  }
  return axios.get('/api/employees', {params:{department_id: departmentId}})
    .then(res => {
      let html = '<option value="">请选择</option>';
      for(const e of res.data) {
        html += `<option value="${e.id}">${e.name}</option>`;
      }
      $select.html(html);
      if (selectedId) $select.val(selectedId);
      return null; // 保证有返回值，then链不会断
    })
    .catch(err => handleApiError(err, '加载员工失败'));
} 