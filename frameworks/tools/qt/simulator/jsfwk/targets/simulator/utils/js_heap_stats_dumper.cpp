/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "js_heap_stats_dumper.h"
#include "ace_log.h"
#include "securec.h"
#include <QDir>

namespace OHOS {
namespace ACELite {
JSHeapStatsDumper::JSHeapStatsDumper()
    : writer_(QDir::currentPath() + "/dump_heap.txt")
{
    if (memset_s(&stats_, sizeof(stats_), 0, sizeof(stats_)) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to memset.");
    }
}

JSHeapStatsDumper::~JSHeapStatsDumper() {}

void JSHeapStatsDumper::Dump()
{
    JSHeapStatus stats;
    if (!JSI::GetJSHeapStatus(stats)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to get JS heap stats.");
        return;
    }
    if (HasChanged(stats)) {
        HandleChange(stats);
    }
}

bool JSHeapStatsDumper::HasChanged(JSHeapStatus &stats) const
{
    return stats.allocBytes != stats_.allocBytes || stats.peakAllocBytes != stats_.peakAllocBytes;
}

void JSHeapStatsDumper::HandleChange(JSHeapStatus &stats)
{
    UpdateStats(stats);
    LogStats(stats);
    SaveStats(stats);
}

void JSHeapStatsDumper::UpdateStats(JSHeapStatus &stats)
{
    stats_.allocBytes = stats.allocBytes;
    stats_.peakAllocBytes = stats.peakAllocBytes;
    stats_.totalBytes = stats.totalBytes;
}

void JSHeapStatsDumper::LogStats(JSHeapStatus &stats) const
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "Current Allocated Bytes: %d, Peak Allocated Bytes: %d, Total Bytes: %d",
                stats.allocBytes, stats.peakAllocBytes, stats.totalBytes);
}

void JSHeapStatsDumper::SaveStats(JSHeapStatus &stats)
{
    char buffer[255];
    sprintf(buffer, "%d/%d", stats.allocBytes, stats.peakAllocBytes);
    writer_.Write(buffer);
}
} // namespace ACELite
} // namespace OHOS
