/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "tools/gpu/ManagedBackendTexture.h"

#include "include/core/SkImageInfo.h"
#include "include/private/GrTypesPriv.h"
#include "src/core/SkMipmap.h"

namespace {

struct Context {
    sk_sp<sk_gpu_test::ManagedBackendTexture> fMBET;
    GrGpuFinishedProc fWrappedProc = nullptr;
    GrGpuFinishedContext fWrappedContext = nullptr;
};

}  // anonymous namespace

namespace sk_gpu_test {

void ManagedBackendTexture::ReleaseProc(void* ctx) {
    std::unique_ptr<Context> context(static_cast<Context*>(ctx));
    if (context->fWrappedProc) {
        context->fWrappedProc(context->fWrappedContext);
    }
}

ManagedBackendTexture::~ManagedBackendTexture() {
    if (fDContext && fTexture.isValid()) {
        fDContext->deleteBackendTexture(fTexture);
    }
}

void* ManagedBackendTexture::releaseContext(GrGpuFinishedProc wrappedProc,
                                            GrGpuFinishedContext wrappedCtx) const {
    // Make sure we don't get a wrapped ctx without a wrapped proc
    SkASSERT(!wrappedCtx || wrappedProc);
    return new Context{sk_ref_sp(this), wrappedProc, wrappedCtx};
}

sk_sp<GrRefCntedCallback> ManagedBackendTexture::refCountedCallback() const {
    return sk_make_sp<GrRefCntedCallback>(ReleaseProc, this->releaseContext());
}

void ManagedBackendTexture::wasAdopted() { fTexture = {}; }

sk_sp<ManagedBackendTexture> ManagedBackendTexture::MakeFromInfo(GrDirectContext* dContext,
                                                                 const SkImageInfo& ii,
                                                                 GrMipmapped mipmapped,
                                                                 GrRenderable renderable,
                                                                 GrProtected isProtected) {
    return MakeWithoutData(
            dContext, ii.width(), ii.height(), ii.colorType(), mipmapped, renderable, isProtected);
}

sk_sp<ManagedBackendTexture> ManagedBackendTexture::MakeFromBitmap(GrDirectContext* dContext,
                                                                   const SkBitmap& bitmap,
                                                                   GrMipmapped mipmapped,
                                                                   GrRenderable renderable,
                                                                   GrProtected isProtected) {
    std::vector<SkPixmap> levels({bitmap.pixmap()});
    std::unique_ptr<SkMipmap> mm;

    if (mipmapped == GrMipmapped::kYes) {
        mm.reset(SkMipmap::Build(bitmap, nullptr));
        if (!mm) {
            return nullptr;
        }
        for (int i = 0; i < mm->countLevels(); ++i) {
            SkMipmap::Level level;
            SkAssertResult(mm->getLevel(i, &level));
            levels.push_back(level.fPixmap);
        }
    }
    return MakeWithData(
            dContext, levels.data(), static_cast<int>(levels.size()), renderable, isProtected);
}

}  // namespace sk_gpu_test
