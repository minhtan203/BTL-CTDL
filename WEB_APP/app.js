'use strict';
/* =============================================================================
 * ĐỀ 19 – ĐẶT VÉ XEM PHIM & HÀNG CHỜ KHÁCH HÀNG (bản Web, chạy hoàn toàn ở trình duyệt)
 * Port từ mã C++ gốc: Deque tự cài đặt bằng danh sách liên kết đôi (KHÔNG dùng Array).
 * ========================================================================== */

// ---------- 1. DEQUE = DANH SÁCH LIÊN KẾT ĐÔI ----------
//  null <- [prev|data|next] <-> [prev|data|next] <-> [prev|data|next] -> null
//             ^ head                                    ^ tail
class Node {
  constructor(data) { this.data = data; this.prev = null; this.next = null; }
}
class Deque {
  constructor() { this.head = null; this.tail = null; this.size = 0; }

  pushFront(d) {                       // chen lên ĐẦU hàng (ưu tiên / hoàn vé)
    const n = new Node(d);
    if (!this.head) this.head = this.tail = n;   // deque rỗng
    else { n.next = this.head; this.head.prev = n; this.head = n; }
    this.size++;
  }
  pushBack(d) {                        // xếp vào CUỐI hàng (vé thường)
    const n = new Node(d);
    if (!this.tail) this.head = this.tail = n;
    else { n.prev = this.tail; this.tail.next = n; this.tail = n; }
    this.size++;
  }
  popFront() {                         // phục vụ khách ở đầu hàng
    if (!this.head) return null;
    const del = this.head;
    this.head = del.next;
    if (this.head) this.head.prev = null; else this.tail = null;
    this.size--;
    return del.data;
  }
  popBack() {                          // khách cuối hàng bỏ về
    if (!this.tail) return null;
    const del = this.tail;
    this.tail = del.prev;
    if (this.tail) this.tail.next = null; else this.head = null;
    this.size--;
    return del.data;
  }
  removeById(id) {                     // xóa nút bất kỳ: nối prev <-> next của hai nút lân cận
    for (let p = this.head; p; p = p.next) {
      if (p.data.id !== id) continue;
      if (p.prev) p.prev.next = p.next; else this.head = p.next;
      if (p.next) p.next.prev = p.prev; else this.tail = p.prev;
      this.size--;
      return p.data;
    }
    return null;
  }
  *[Symbol.iterator]() { for (let p = this.head; p; p = p.next) yield p; } // duyệt head -> tail
}

// ---------- 2. DỮ LIỆU & HẰNG SỐ ----------
const ROWS = 8, COLS = 10, VIP_FROM = 2, VIP_TO = 5, MAX_SEATS = 8;
const PRICE_N = 80000, PRICE_V = 120000, REFUND_PCT = 80;
const $ = id => document.getElementById(id);
const esc = s => String(s).replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const money = n => n.toLocaleString('vi-VN') + ' đ';
const isVip = r => r >= VIP_FROM && r <= VIP_TO;
const sName = ([r, c]) => String.fromCharCode(65 + r) + (c + 1);
const sPrice = ([r]) => isVip(r) ? PRICE_V : PRICE_N;
const sList = a => a.map(sName).join(', ');

const queue = new Deque();
const tickets = [];
const seats = Array.from({ length: ROWS }, () => Array.from({ length: COLS }, () => ({ tk: 0 })));
let nextCus = 1, nextTk = 1;
let serving = null, picked = [], hint = [], flash = 0, busy = false;

// ---------- 3. GỢI Ý GHẾ ----------
// Ưu tiên n ghế LIỀN NHAU cùng hàng, gần trung tâm phòng nhất; không có thì lấy ghế rời.
function suggest(n, vip) {
  const midR = (ROWS - 1) / 2, midC = (COLS - 1) / 2;
  let best = [], bs = Infinity;
  for (let r = 0; r < ROWS; r++) {
    if (isVip(r) !== vip) continue;
    for (let s = 0; s + n <= COLS; s++) {
      let ok = true;
      for (let k = 0; k < n; k++) if (seats[r][s + k].tk) { ok = false; break; }
      if (!ok) continue;
      const sc = Math.abs(r - midR) * 100 + Math.abs(s + (n - 1) / 2 - midC) * 10;
      if (sc < bs) { bs = sc; best = Array.from({ length: n }, (_, k) => [r, s + k]); }
    }
  }
  if (best.length) return best;
  const all = [];
  for (let r = 0; r < ROWS; r++) for (let c = 0; c < COLS; c++)
    if (!seats[r][c].tk && isVip(r) === vip) all.push([r, c]);
  if (all.length < n) return [];
  const d = a => Math.abs(a[0] - midR) * 100 + Math.abs(a[1] - midC);
  return all.sort((a, b) => d(a) - d(b)).slice(0, n);
}

// ---------- 4. NGHIỆP VỤ ----------
function toast(msg, kind) {
  const t = $('toast'); t.textContent = msg; t.className = 'show ' + (kind || '');
  clearTimeout(toast.h); toast.h = setTimeout(() => t.className = '', 3000);
}
const leave = id => new Promise(res => {          // hiệu ứng nút biến mất trước khi xóa khỏi deque
  const el = document.querySelector(`[data-id="${id}"]`);
  if (!el) return res();
  el.classList.add('leave'); setTimeout(res, 350);
});
function newCustomer(o) { return Object.assign({ id: nextCus++, type: 'normal', n: 1, vip: false, refundTk: 0 }, o); }

function addCustomer(c) {
  if (c.type === 'priority') queue.pushFront(c); else queue.pushBack(c);
  flash = c.id;
}
function makeTicket(c, list, pay) {
  const t = { id: nextTk++, name: c.name, phone: c.phone, seats: list, total: list.reduce((s, x) => s + sPrice(x), 0),
              pay, time: new Date().toLocaleString('vi-VN'), active: true, pending: false };
  list.forEach(([r, k]) => seats[r][k].tk = t.id);
  tickets.push(t);
  return t;
}
const onDropped = c => { if (c.type === 'refund') { const t = tickets.find(x => x.id === c.refundTk); if (t) t.pending = false; } };

async function serveNext() {
  if (busy) return;
  if (!queue.head) return toast('Hàng chờ đang trống.', 'warn');
  if (serving) return toast('Hãy hoàn tất khách đang được phục vụ trước.', 'warn');
  busy = true; await leave(queue.head.data.id);
  const c = queue.popFront(); busy = false;                        // pop_front
  if (c.type === 'refund') doRefund(c);
  else {
    serving = c; hint = [];
    picked = suggest(c.n, c.vip);
    toast(picked.length ? `Đang phục vụ ${c.name}. Đã gợi ý sẵn ${picked.length} ghế.` : 'Không còn đủ ghế để gợi ý, hãy tự chọn.', picked.length ? '' : 'warn');
  }
  render();
}
async function dropLast() {
  if (busy) return;
  if (!queue.tail) return toast('Hàng chờ đang trống.', 'warn');
  busy = true; await leave(queue.tail.data.id);
  const c = queue.popBack(); busy = false;                         // pop_back
  onDropped(c); toast(`Khách cuối hàng #${c.id} (${c.name}) đã bỏ về.`); render();
}
async function removeCustomer() {
  const id = +$('rm-id').value;
  if (busy || !id) return toast('Nhập ID khách cần xóa.', 'warn');
  const exists = [...queue].some(p => p.data.id === id);
  if (!exists) return toast(`Không có khách #${id} trong hàng chờ.`, 'warn');
  busy = true; await leave(id);
  const c = queue.removeById(id); busy = false;
  onDropped(c); $('rm-id').value = ''; toast(`Đã xóa khách #${id} (${c.name}) khỏi hàng chờ.`); render();
}
function doRefund(c) {
  const t = tickets.find(x => x.id === c.refundTk);
  if (!t || !t.active) return toast('Vé này không còn hiệu lực để hoàn.', 'warn');
  t.seats.forEach(([r, k]) => seats[r][k].tk = 0);                 // trả ghế về trống
  t.active = false; t.pending = false;
  toast(`Đã hoàn vé #${t.id}: trả ghế ${sList(t.seats)}, hoàn ${money(t.total * REFUND_PCT / 100)}.`);
}
function cancelTicket(id) {
  const t = tickets.find(x => x.id === id);
  if (!t || !t.active || t.pending) return;
  if (!confirm(`Hủy vé #${t.id}? Hoàn ${REFUND_PCT}% = ${money(t.total * REFUND_PCT / 100)}.`)) return;
  t.pending = true;
  addCustomer(newCustomer({ name: t.name, phone: t.phone, type: 'refund', refundTk: t.id }));  // push_front
  toast(`Yêu cầu hoàn vé #${t.id} đã được đẩy lên ĐẦU hàng chờ.`); render();
}
function pay() {
  if (picked.length !== serving.n) return toast(`Cần chọn đúng ${serving.n} ghế (đang chọn ${picked.length}).`, 'warn');
  const t = makeTicket(serving, picked, $('pay').value);
  toast(`Đặt vé thành công! Mã vé #${t.id}, ghế ${sList(t.seats)}.`);
  serving = null; picked = []; render();
}
function skip() { toast(`${serving.name} đã rời đi, không đặt vé.`); serving = null; picked = []; render(); }

function seatClick(r, c) {
  if (seats[r][c].tk) return toast('Ghế này đã có người đặt.', 'warn');
  if (!serving) return toast('Bấm "Phục vụ khách kế tiếp" để bắt đầu chọn ghế cho khách.', 'warn');
  const i = picked.findIndex(([a, b]) => a === r && b === c);
  if (i >= 0) picked.splice(i, 1);
  else if (picked.length >= serving.n) return toast(`Khách chỉ đặt ${serving.n} ghế. Bỏ bớt một ghế trước.`, 'warn');
  else picked.push([r, c]);
  render();
}

// ---------- 5. XUẤT VÉ ----------
function ticketText(t) {
  const L = '='.repeat(46);
  return [L, '        VÉ XEM PHIM / HÓA ĐƠN', L, `Mã vé      : #${t.id}`, 'Suất chiếu : 19:30 - Phòng 01',
    `Khách hàng : ${t.name}`, `Điện thoại : ${t.phone}`, `Ngày lập   : ${t.time}`, '-'.repeat(46),
    ...t.seats.map(s => `  Ghế ${sName(s).padEnd(4)} ${isVip(s[0]) ? 'VIP    ' : 'Thường '}${money(sPrice(s))}`),
    '-'.repeat(46), `TỔNG TIỀN  : ${money(t.total)}`, `Thanh toán : ${t.pay}`,
    `Trạng thái : ${t.active ? 'Hiệu lực' : 'Đã hoàn/hủy (hoàn ' + money(t.total * REFUND_PCT / 100) + ')'}`, L].join('\n');
}
function downloadTicket(id) {
  const t = tickets.find(x => x.id === id);
  const a = document.createElement('a');
  a.href = URL.createObjectURL(new Blob([ticketText(t)], { type: 'text/plain;charset=utf-8' }));
  a.download = `ve_${id}.txt`; a.click(); URL.revokeObjectURL(a.href);
}
function printTicket(id) {
  const t = tickets.find(x => x.id === id), w = window.open('', '_blank');
  if (!w) return toast('Trình duyệt đang chặn cửa sổ in.', 'warn');
  w.document.write(`<title>Vé #${id}</title><pre style="font:15px/1.5 Consolas,monospace">${esc(ticketText(t))}</pre>`);
  w.document.close(); w.focus(); w.print();
}

// ---------- 6. HIỂN THỊ ----------
function renderQueue() {
  $('qsize').textContent = queue.size + ' khách';
  let h = '';
  for (const p of queue) {                                         // duyệt head -> tail qua next
    const d = p.data;
    const detail = d.type === 'refund' ? `Hoàn vé #${d.refundTk}` : `${d.n} ghế ${d.vip ? 'VIP' : 'thường'}`;
    h += `<div class="lnk">⇄</div><div class="node ${d.type} ${d.id === flash ? 'enter' : ''}" data-id="${d.id}">
      <div class="tags">${p === queue.head ? '<b class="head">HEAD</b>' : ''}${p === queue.tail ? '<b class="tail">TAIL</b>' : ''}</div>
      <div class="nid">#${d.id}</div><div class="nm" title="${esc(d.name)}">${esc(d.name)}</div>
      <div class="sub">${{ normal: 'Vé thường', priority: 'Ưu tiên', refund: 'Hoàn vé' }[d.type]}</div><div class="sub">${detail}</div>
      <div class="ptr">prev: ${p.prev ? '#' + p.prev.data.id : 'null'}<br>next: ${p.next ? '#' + p.next.data.id : 'null'}</div></div>`;
  }
  $('viz').innerHTML = queue.head
    ? `<div class="end">head.prev<br>= null</div>${h}<div class="lnk">⇄</div><div class="end">tail.next<br>= null</div>`
    : '<div class="empty">Hàng chờ trống: head = null, tail = null. Hãy thêm khách bên dưới.</div>';
  flash = 0;
}
function renderSeats() {
  const sel = serving ? picked : hint;
  let h = '<span></span>' + Array.from({ length: COLS }, (_, c) => `<span class="lab">${c + 1}</span>`).join('');
  for (let r = 0; r < ROWS; r++) {
    h += `<span class="lab">${String.fromCharCode(65 + r)}</span>`;
    for (let c = 0; c < COLS; c++) {
      const cls = seats[r][c].tk ? 'booked' : sel.some(([a, b]) => a === r && b === c) ? 'pick' : isVip(r) ? 'vip' : '';
      h += `<button class="seat ${cls}" data-r="${r}" data-c="${c}" aria-label="Ghế ${sName([r, c])}">${sName([r, c])}</button>`;
    }
  }
  $('map').innerHTML = h;
  const b = $('banner');
  b.className = 'banner' + (serving ? ' on' : '');
  b.textContent = serving
    ? `Đang phục vụ #${serving.id} ${serving.name} (${serving.phone}): cần ${serving.n} ghế ${serving.vip ? 'VIP' : 'thường'}. Đã chọn ${picked.length}/${serving.n}${picked.length ? ': ' + sList(picked) + ' – ' + money(picked.reduce((s, x) => s + sPrice(x), 0)) : ''}. Bấm vào ghế để đổi.`
    : 'Chưa phục vụ khách nào. Bấm "Phục vụ khách kế tiếp" để khách ở đầu hàng bắt đầu chọn ghế.';
  $('book-ctl').hidden = !serving; $('idle-ctl').hidden = !!serving;
}
function renderTickets() {
  $('tickets').innerHTML = tickets.length ? tickets.slice().reverse().map(t => {
    const st = !t.active ? ['off', 'Đã hoàn'] : t.pending ? ['wait', 'Chờ hoàn'] : ['ok', 'Hiệu lực'];
    return `<tr><td>#${t.id}</td><td>${esc(t.name)}<br><small>${esc(t.phone)}</small></td><td>${sList(t.seats)}</td>
      <td>${money(t.total)}</td><td class="st ${st[0]}">${st[1]}</td>
      <td><div class="act"><button data-act="cancel" data-id="${t.id}" class="danger" ${t.active && !t.pending ? '' : 'disabled'}>Hủy vé</button>
      <button data-act="txt" data-id="${t.id}">.txt</button><button data-act="print" data-id="${t.id}">In / PDF</button></div></td></tr>`;
  }).join('') : '<tr><td colspan="6">Chưa có vé nào.</td></tr>';
}
function render() { renderQueue(); renderSeats(); renderTickets(); }

// ---------- 7. SỰ KIỆN ----------
$('btn-serve').onclick = serveNext;
$('btn-popback').onclick = dropLast;
$('btn-rm').onclick = removeCustomer;
$('btn-pay').onclick = pay;
$('btn-skip').onclick = skip;
$('btn-accept').onclick = () => { picked = suggest(serving.n, serving.vip); if (!picked.length) toast('Không còn đủ ghế để gợi ý.', 'warn'); render(); };
$('btn-hint').onclick = () => {
  const n = Math.min(MAX_SEATS, Math.max(1, +$('h-n').value || 1)), v = $('h-vip').value === '1';
  hint = suggest(n, v); toast(hint.length ? `Gợi ý: ${sList(hint)}` : `Không đủ ${n} ghế ${v ? 'VIP' : 'thường'} trống.`, hint.length ? '' : 'warn'); render();
};
$('map').onclick = e => { const b = e.target.closest('.seat'); if (b) seatClick(+b.dataset.r, +b.dataset.c); };
$('tickets').onclick = e => {
  const b = e.target.closest('button'); if (!b) return;
  const id = +b.dataset.id;
  ({ cancel: cancelTicket, txt: downloadTicket, print: printTicket })[b.dataset.act](id);
};
$('add-form').onsubmit = e => {
  e.preventDefault();
  const c = newCustomer({ name: $('f-name').value.trim(), phone: $('f-phone').value.trim(), type: $('f-type').value,
    n: Math.min(MAX_SEATS, Math.max(1, +$('f-n').value || 1)), vip: $('f-vip').value === '1' });
  addCustomer(c); e.target.reset(); $('f-n').value = 2;
  toast(`Đã thêm #${c.id} ${c.name} vào ${c.type === 'priority' ? 'ĐẦU' : 'CUỐI'} hàng chờ.`); render();
};

// ---------- 8. DỮ LIỆU MẪU ----------
[['Nguyễn Văn An', '0901234567', [[3, 4], [3, 5]]], ['Trần Thị Bình', '0912345678', [[1, 1], [1, 2], [1, 3]]],
 ['Lê Quốc Cường', '0987654321', [[5, 7], [5, 8]]]]
  .forEach(([name, phone, list]) => makeTicket(newCustomer({ name, phone }), list, 'Tiền mặt'));
[['Phạm Minh Đức', '0933111222', 'normal', 2, false], ['Võ Thu Hà', '0944222333', 'normal', 3, true],
 ['Đặng Gia Huy', '0955333444', 'priority', 1, true], ['Bùi Lan Khanh', '0966444555', 'normal', 4, false]]
  .forEach(([name, phone, type, n, vip]) => addCustomer(newCustomer({ name, phone, type, n, vip })));
flash = 0;
render();
