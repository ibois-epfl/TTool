#pragma once

// @generated by torchgen/gen.py from Function.h

#include <ATen/Context.h>
#include <ATen/DeviceGuard.h>
#include <ATen/TensorUtils.h>
#include <ATen/TracerMode.h>
#include <ATen/core/Generator.h>
#include <ATen/core/Reduction.h>
#include <ATen/core/Tensor.h>
#include <c10/core/Scalar.h>
#include <c10/core/Storage.h>
#include <c10/core/TensorOptions.h>
#include <c10/util/Deprecated.h>
#include <c10/util/Optional.h>



#include <ATen/ops/new_empty_strided_ops.h>

namespace at {


namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, int64_t>::value>>
  at::Tensor new_empty_strided(const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride, at::TensorOptions options={}) {
    return at::_ops::new_empty_strided::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), optTypeMetaToScalarType(options.dtype_opt()), options.layout_opt(), options.device_opt(), options.pinned_memory_opt());
  }
}

namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, int64_t>::value>>
  at::Tensor new_empty_strided(const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory) {
    return at::_ops::new_empty_strided::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), dtype, layout, device, pin_memory);
  }
}

namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, c10::SymInt>::value>>
  at::Tensor new_empty_strided(const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride, at::TensorOptions options={}) {
    return at::_ops::new_empty_strided::call(self, size, stride, optTypeMetaToScalarType(options.dtype_opt()), options.layout_opt(), options.device_opt(), options.pinned_memory_opt());
  }
}

namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, c10::SymInt>::value>>
  at::Tensor new_empty_strided(const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory) {
    return at::_ops::new_empty_strided::call(self, size, stride, dtype, layout, device, pin_memory);
  }
}

// aten::new_empty_strided.out(Tensor self, SymInt[] size, SymInt[] stride, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & new_empty_strided_out(at::Tensor & out, const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride) {
    return at::_ops::new_empty_strided_out::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), out);
}
namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, int64_t>::value>>
  at::Tensor & new_empty_strided_out(at::Tensor & out, const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride) {
    return at::_ops::new_empty_strided_out::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), out);
  }
}

// aten::new_empty_strided.out(Tensor self, SymInt[] size, SymInt[] stride, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & new_empty_strided_outf(const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride, at::Tensor & out) {
    return at::_ops::new_empty_strided_out::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), out);
}
namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, int64_t>::value>>
  at::Tensor & new_empty_strided_outf(const at::Tensor & self, at::IntArrayRef size, at::IntArrayRef stride, at::Tensor & out) {
    return at::_ops::new_empty_strided_out::call(self, c10::fromIntArrayRefSlow(size), c10::fromIntArrayRefSlow(stride), out);
  }
}

// aten::new_empty_strided.out(Tensor self, SymInt[] size, SymInt[] stride, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & new_empty_strided_symint_out(at::Tensor & out, const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride) {
    return at::_ops::new_empty_strided_out::call(self, size, stride, out);
}
namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, c10::SymInt>::value>>
  at::Tensor & new_empty_strided_out(at::Tensor & out, const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride) {
    return at::_ops::new_empty_strided_out::call(self, size, stride, out);
  }
}

// aten::new_empty_strided.out(Tensor self, SymInt[] size, SymInt[] stride, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & new_empty_strided_symint_outf(const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride, at::Tensor & out) {
    return at::_ops::new_empty_strided_out::call(self, size, stride, out);
}
namespace symint {
  template <typename T, typename = std::enable_if_t<std::is_same<T, c10::SymInt>::value>>
  at::Tensor & new_empty_strided_outf(const at::Tensor & self, c10::SymIntArrayRef size, c10::SymIntArrayRef stride, at::Tensor & out) {
    return at::_ops::new_empty_strided_out::call(self, size, stride, out);
  }
}

}
