/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef XLA_SERVICE_GPU_GEMM_REWRITER_TRITON_H_
#define XLA_SERVICE_GPU_GEMM_REWRITER_TRITON_H_

// This file contains the code for fusing dots and other operations into Triton
// GEMM fusions.

#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/hlo/ir/hlo_instructions.h"
#include "xla/service/hlo_pass_interface.h"
#include "xla/service/instruction_fusion.h"
#include "xla/statusor.h"
#include "xla/stream_executor/device_description.h"

namespace xla {
namespace gpu {

// Filters GEMMs which can be handled using Triton.
FusionDecision CanTritonHandleGEMM(const HloDotInstruction&,
                                   const se::GpuComputeCapability&);

// Filters GEMMs which are better to handle using Triton.
bool ShouldTritonHandleGEMM(HloDotInstruction&,
                            const se::GpuComputeCapability&);

// Rewrite compatible dot() calls into custom calls with fused computations
// that target Triton-based matmul emitter.
class GemmRewriterTriton : public HloModulePass {
 public:
  explicit GemmRewriterTriton(const se::GpuComputeCapability& gpu_version)
      : gpu_version_(gpu_version) {}
  absl::string_view name() const override { return "triton-gemm-rewriter"; }

  using HloPassInterface::Run;
  absl::StatusOr<bool> Run(
      HloModule* module,
      const absl::flat_hash_set<absl::string_view>& execution_threads) override;

 private:
  se::GpuComputeCapability gpu_version_;
};

}  // namespace gpu
}  // namespace xla

#endif  // XLA_SERVICE_GPU_GEMM_REWRITER_TRITON_H_
