#pragma once

// @generated by torchgen/gen.py from Operator.h

#include <tuple>
#include <vector>

// Forward declarations of any types needed in the operator signatures.
// We can't directly include these classes because it will cause circular include dependencies.
// This file is included by TensorBody.h, which defines the Tensor class.
#include <ATen/core/ATen_fwd.h>

namespace at {
namespace _ops {


struct TORCH_API randint_like {
  using schema = at::Tensor (const at::Tensor &, int64_t, c10::optional<at::ScalarType>, c10::optional<at::Layout>, c10::optional<at::Device>, c10::optional<bool>, c10::optional<at::MemoryFormat>);
  using ptr_schema = schema*;
  // See Note [static constexpr char* members for windows NVCC]
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(name, "aten::randint_like")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(overload_name, "")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(schema_str, "randint_like(Tensor self, int high, *, ScalarType? dtype=None, Layout? layout=None, Device? device=None, bool? pin_memory=None, MemoryFormat? memory_format=None) -> Tensor")
  static at::Tensor call(const at::Tensor & self, int64_t high, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, c10::optional<at::MemoryFormat> memory_format);
  static at::Tensor redispatch(c10::DispatchKeySet dispatchKeySet, const at::Tensor & self, int64_t high, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, c10::optional<at::MemoryFormat> memory_format);
};

struct TORCH_API randint_like_low_dtype {
  using schema = at::Tensor (const at::Tensor &, int64_t, int64_t, c10::optional<at::ScalarType>, c10::optional<at::Layout>, c10::optional<at::Device>, c10::optional<bool>, c10::optional<at::MemoryFormat>);
  using ptr_schema = schema*;
  // See Note [static constexpr char* members for windows NVCC]
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(name, "aten::randint_like")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(overload_name, "low_dtype")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(schema_str, "randint_like.low_dtype(Tensor self, int low, int high, *, ScalarType? dtype=None, Layout? layout=None, Device? device=None, bool? pin_memory=None, MemoryFormat? memory_format=None) -> Tensor")
  static at::Tensor call(const at::Tensor & self, int64_t low, int64_t high, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, c10::optional<at::MemoryFormat> memory_format);
  static at::Tensor redispatch(c10::DispatchKeySet dispatchKeySet, const at::Tensor & self, int64_t low, int64_t high, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, c10::optional<at::MemoryFormat> memory_format);
};

struct TORCH_API randint_like_out {
  using schema = at::Tensor & (const at::Tensor &, int64_t, c10::optional<at::MemoryFormat>, at::Tensor &);
  using ptr_schema = schema*;
  // See Note [static constexpr char* members for windows NVCC]
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(name, "aten::randint_like")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(overload_name, "out")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(schema_str, "randint_like.out(Tensor self, int high, *, MemoryFormat? memory_format=None, Tensor(a!) out) -> Tensor(a!)")
  static at::Tensor & call(const at::Tensor & self, int64_t high, c10::optional<at::MemoryFormat> memory_format, at::Tensor & out);
  static at::Tensor & redispatch(c10::DispatchKeySet dispatchKeySet, const at::Tensor & self, int64_t high, c10::optional<at::MemoryFormat> memory_format, at::Tensor & out);
};

struct TORCH_API randint_like_low_dtype_out {
  using schema = at::Tensor & (const at::Tensor &, int64_t, int64_t, c10::optional<at::MemoryFormat>, at::Tensor &);
  using ptr_schema = schema*;
  // See Note [static constexpr char* members for windows NVCC]
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(name, "aten::randint_like")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(overload_name, "low_dtype_out")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(schema_str, "randint_like.low_dtype_out(Tensor self, int low, int high, *, MemoryFormat? memory_format=None, Tensor(a!) out) -> Tensor(a!)")
  static at::Tensor & call(const at::Tensor & self, int64_t low, int64_t high, c10::optional<at::MemoryFormat> memory_format, at::Tensor & out);
  static at::Tensor & redispatch(c10::DispatchKeySet dispatchKeySet, const at::Tensor & self, int64_t low, int64_t high, c10::optional<at::MemoryFormat> memory_format, at::Tensor & out);
};

}} // namespace at::_ops
