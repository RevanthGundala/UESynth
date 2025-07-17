// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "MockGrpcContext.h"

MockGrpcContext::MockGrpcContext()
    : bIsCancelled(false), bIsDeadlineExceeded(false) {}

MockGrpcContext::~MockGrpcContext() {}