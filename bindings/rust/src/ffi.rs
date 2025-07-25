#![allow(non_camel_case_types)]
#![allow(dead_code)]

use crate::{Unicorn, UnicornInner};

use super::unicorn_const::{uc_error, Arch, HookType, MemRegion, MemType, Mode, Query, TlbEntry};
use alloc::rc::Weak;
use core::cell::UnsafeCell;
use core::ffi::c_void;
use libc::{c_char, c_int};

pub type uc_handle = *mut c_void;
// TODO: Use c_size_t as soon as it is stable. The c api exposes uc_hook as size_t
pub type uc_hook = *mut c_void;
pub type uc_context = *mut c_void;

extern "C" {
    pub fn uc_version(major: *mut u32, minor: *mut u32) -> u32;
    pub fn uc_arch_supported(arch: Arch) -> bool;
    pub fn uc_open(arch: Arch, mode: Mode, engine: *mut uc_handle) -> uc_error;
    pub fn uc_close(engine: uc_handle) -> uc_error;
    pub fn uc_context_free(mem: uc_context) -> uc_error;
    pub fn uc_errno(engine: uc_handle) -> uc_error;
    pub fn uc_strerror(error_code: uc_error) -> *const c_char;
    pub fn uc_reg_write(engine: uc_handle, regid: c_int, value: *const c_void) -> uc_error;
    pub fn uc_reg_write_batch(
        engine: uc_handle,
        regids: *const c_int,
        values: *const *const c_void,
        count: c_int,
    ) -> uc_error;
    pub fn uc_reg_read(engine: uc_handle, regid: c_int, value: *mut c_void) -> uc_error;
    pub fn uc_reg_read_batch(
        engine: uc_handle,
        regids: *const c_int,
        values: *const *mut c_void,
        count: c_int,
    ) -> uc_error;
    pub fn uc_mem_write(
        engine: uc_handle,
        address: u64,
        bytes: *const u8,
        size: u64,
    ) -> uc_error;
    pub fn uc_mem_read(
        engine: uc_handle,
        address: u64,
        bytes: *mut u8,
        size: u64,
    ) -> uc_error;
    pub fn uc_vmem_read(
        engine: uc_handle,
        address: u64,
        prot: u32,
        bytes: *mut u8,
        size: libc::size_t,
    ) -> uc_error;
    pub fn uc_vmem_translate(
        engine: uc_handle,
        address: u64,
        prot: u32,
        paddr: *mut u64,
    ) -> uc_error;
    pub fn uc_mem_map(engine: uc_handle, address: u64, size: u64, perms: u32) -> uc_error;
    pub fn uc_mem_map_ptr(
        engine: uc_handle,
        address: u64,
        size: u64,
        perms: u32,
        ptr: *mut c_void,
    ) -> uc_error;
    pub fn uc_mmio_map(
        engine: uc_handle,
        address: u64,
        size: u64,
        read_cb: *mut c_void,
        user_data_read: *mut c_void,
        write_cb: *mut c_void,
        user_data_write: *mut c_void,
    ) -> uc_error;
    pub fn uc_mem_unmap(engine: uc_handle, address: u64, size: u64) -> uc_error;
    pub fn uc_mem_protect(
        engine: uc_handle,
        address: u64,
        size: u64,
        perms: u32,
    ) -> uc_error;
    pub fn uc_mem_regions(
        engine: uc_handle,
        regions: *const *const MemRegion,
        count: *mut u32,
    ) -> uc_error;
    pub fn uc_emu_start(
        engine: uc_handle,
        begin: u64,
        until: u64,
        timeout: u64,
        count: libc::size_t,
    ) -> uc_error;
    pub fn uc_emu_stop(engine: uc_handle) -> uc_error;
    pub fn uc_hook_add(
        engine: uc_handle,
        hook: *mut uc_hook,
        hook_type: HookType,
        callback: *mut c_void,
        user_data: *mut c_void,
        begin: u64,
        end: u64,
        ...
    ) -> uc_error;
    pub fn uc_hook_del(engine: uc_handle, hook: uc_hook) -> uc_error;
    pub fn uc_query(engine: uc_handle, query_type: Query, result: *mut libc::size_t) -> uc_error;
    pub fn uc_context_alloc(engine: uc_handle, context: *mut uc_context) -> uc_error;
    pub fn uc_context_save(engine: uc_handle, context: uc_context) -> uc_error;
    pub fn uc_context_restore(engine: uc_handle, context: uc_context) -> uc_error;
    pub fn uc_ctl(engine: uc_handle, control: u32, ...) -> uc_error;
}

pub struct UcHook<'a, D: 'a, F: 'a> {
    pub callback: F,
    pub uc: Weak<UnsafeCell<UnicornInner<'a, D>>>,
}

pub trait IsUcHook<'a> {}

impl<'a, D, F> IsUcHook<'a> for UcHook<'a, D, F> {}

pub unsafe extern "C" fn mmio_read_callback_proxy<D, F>(
    uc: uc_handle,
    offset: u64,
    size: usize,
    user_data: *mut UcHook<D, F>,
) -> u64
where
    F: FnMut(&mut crate::Unicorn<D>, u64, usize) -> u64,
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, offset, size)
}

pub unsafe extern "C" fn mmio_write_callback_proxy<D, F>(
    uc: uc_handle,
    offset: u64,
    size: usize,
    value: u64,
    user_data: *mut UcHook<D, F>,
) where
    F: FnMut(&mut crate::Unicorn<D>, u64, usize, u64),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, offset, size, value);
}

pub unsafe extern "C" fn code_hook_proxy<D, F>(
    uc: uc_handle,
    address: u64,
    size: u32,
    user_data: *mut UcHook<D, F>,
) where
    F: FnMut(&mut crate::Unicorn<D>, u64, u32),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, address, size);
}

pub unsafe extern "C" fn block_hook_proxy<D, F>(
    uc: uc_handle,
    address: u64,
    size: u32,
    user_data: *mut UcHook<D, F>,
) where
    F: FnMut(&mut crate::Unicorn<D>, u64, u32),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, address, size);
}

pub unsafe extern "C" fn mem_hook_proxy<D, F>(
    uc: uc_handle,
    mem_type: MemType,
    address: u64,
    size: u32,
    value: i64,
    user_data: *mut UcHook<D, F>,
) -> bool
where
    F: FnMut(&mut crate::Unicorn<D>, MemType, u64, usize, i64) -> bool,
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, mem_type, address, size as usize, value)
}

pub unsafe extern "C" fn intr_hook_proxy<D, F>(
    uc: uc_handle,
    value: u32,
    user_data: *mut UcHook<D, F>,
) where
    F: FnMut(&mut crate::Unicorn<D>, u32),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, value);
}

pub unsafe extern "C" fn insn_in_hook_proxy<D, F>(
    uc: uc_handle,
    port: u32,
    size: usize,
    user_data: *mut UcHook<D, F>,
) -> u32
where
    F: FnMut(&mut crate::Unicorn<D>, u32, usize) -> u32,
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, port, size)
}

pub unsafe extern "C" fn insn_invalid_hook_proxy<D, F>(
    uc: uc_handle,
    user_data: *mut UcHook<D, F>,
) -> bool
where
    F: FnMut(&mut crate::Unicorn<D>) -> bool,
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc)
}

pub unsafe extern "C" fn insn_out_hook_proxy<D, F>(
    uc: uc_handle,
    port: u32,
    size: usize,
    value: u32,
    user_data: *mut UcHook<D, F>,
) where
    F: FnMut(&mut crate::Unicorn<D>, u32, usize, u32),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc, port, size, value);
}

pub unsafe extern "C" fn insn_sys_hook_proxy<D, F>(uc: uc_handle, user_data: *mut UcHook<D, F>)
where
    F: FnMut(&mut crate::Unicorn<D>),
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    (user_data.callback)(&mut user_data_uc);
}

pub unsafe extern "C" fn tlb_lookup_hook_proxy<D, F>(
    uc: uc_handle,
    vaddr: u64,
    mem_type: MemType,
    result: *mut TlbEntry,
    user_data: *mut UcHook<D, F>,
) -> bool
where
    F: FnMut(&mut crate::Unicorn<D>, u64, MemType) -> Option<TlbEntry>,
{
    let user_data = &mut *user_data;
    let mut user_data_uc = Unicorn {
        inner: user_data.uc.upgrade().unwrap(),
    };
    debug_assert_eq!(uc, user_data_uc.get_handle());
    let r = (user_data.callback)(&mut user_data_uc, vaddr, mem_type);
    if let Some(ref e) = r {
        let ref_result: &mut TlbEntry = &mut *result;
        *ref_result = *e;
    };
    r.is_some()
}
