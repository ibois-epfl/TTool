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



#include <ATen/ops/floor_divide_ops.h>

namespace at {


// aten::floor_divide(Tensor self, Tensor other) -> Tensor
inline at::Tensor floor_divide(const at::Tensor & self, const at::Tensor & other) {
    return at::_ops::floor_divide::call(self, other);
}

// aten::floor_divide.out(Tensor self, Tensor other, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & floor_divide_out(at::Tensor & out, const at::Tensor & self, const at::Tensor & other) {
    return at::_ops::floor_divide_out::call(self, other, out);
}
// aten::floor_divide.out(Tensor self, Tensor other, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & floor_divide_outf(const at::Tensor & self, const at::Tensor & other, at::Tensor & out) {
    return at::_ops::floor_divide_out::call(self, other, out);
}

// aten::floor_divide.Scalar(Tensor self, Scalar other) -> Tensor
inline at::Tensor floor_divide(const at::Tensor & self, const at::Scalar & other) {
    return at::_ops::floor_divide_Scalar::call(self, other);
}

}
