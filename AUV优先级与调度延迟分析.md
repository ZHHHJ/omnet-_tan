# AUV 优先级与调度延迟分析

## 一、AUV 优先级的物理意义

### 1.1 水声网络中的 AUV 角色

在水声传感器网络中，AUV（自主水下航行器）具有以下特点：

1. **移动性**：AUV 在水下移动，位置不断变化
2. **时间窗口有限**：AUV 在某个传感器附近的时间窗口有限
3. **数据收集任务**：AUV 负责收集传感器数据并带回水面
4. **通信机会稀缺**：由于移动性，AUV 与传感器之间的通信机会有限

### 1.2 AUV 优先级的物理意义

#### 1.2.1 时间敏感性

```
传感器位置固定 ──┐
                │  AUV 移动轨迹
AUV 当前位置 ───┘  ──────>  AUV 未来位置
     ↑                    ↑
  通信窗口1             通信窗口2
  (有限时间)           (有限时间)
```

**物理意义**：

- AUV 在传感器通信范围内的时间窗口有限
- 如果错过当前通信窗口，需要等待 AUV 再次接近
- 等待时间可能很长（取决于 AUV 的移动速度和轨迹）

#### 1.2.2 数据时效性

- **传感器数据**：通常可以等待，延迟几分钟甚至更长时间影响不大
- **AUV 数据收集**：必须在 AUV 离开通信范围前完成，否则数据丢失

#### 1.2.3 网络效率

- AUV 作为移动数据收集节点，优先服务可以提高整体网络效率
- 减少数据重传和丢失，降低网络能耗

### 1.3 代码中的 AUV 优先级实现

#### 1.3.1 分组算法中的优先级（v2 版本）

```cpp
// 按优先级排序扇形
if (auv) {
    int auvId = auv->getId();
    std::sort(sector_order.begin(), sector_order.end(),
             [&](int a, int b) {
                 bool a_is_auv = (sectors[a].destID == auvId);
                 bool b_is_auv = (sectors[b].destID == auvId);
                 if (a_is_auv != b_is_auv) return a_is_auv;  // AUV请求优先
                 return sectors[a].distance < sectors[b].distance;
             });
}
```

**效果**：AUV 请求在初始解生成时优先分配，更容易分配到较小的组（组号更小）

#### 1.3.2 能量函数中的奖励机制

```cpp
// AUV请求优先级奖励
if (auv) {
    int auvId = auv->getId();
    for (int i = 0; i < n; ++i) {
        if (sectors[i].destID == auvId) {
            if (group_sizes[solution[i]] <= 3) {
                energy -= 0.5;  // 奖励：AUV请求在小组中
            }
        }
    }
}
```

**效果**：鼓励将 AUV 请求分配到较小的组（≤3 个成员），从而：

- 减少组内冲突
- 降低组内最大延迟
- 提高调度效率

## 二、调度机制分析

### 2.1 时间槽结构

根据配置文件：

- `slotTime1 = 13s`：Server 接收 RTS 的时间段
- `slotTime2 = 7s`：Server 发送 CTS 的时间段
- `slotSum = slotTime1 + slotTime2 = 20s`：一个完整的时间槽

### 2.2 组间调度机制

```cpp
// 调度时间的安排：每组的data的发生时间累加
adjustedGroupDelay = adjustedGroupDelay + groupInMaxDelay;
```

**关键机制**：

1. **组内并发**：同一组内的所有数据包同时发送（无冲突）
2. **组间串行**：不同组的数据包按顺序发送
3. **延迟累加**：后一组的发送时间 = 前一组的发送时间 + 前一组的最大延迟

### 2.3 调度时间计算

假设有 G 个组，每组最大延迟为 `delay_i`：

```
组1: adjustedGroupDelay_1 = base_time
组2: adjustedGroupDelay_2 = adjustedGroupDelay_1 + max_delay_1
组3: adjustedGroupDelay_3 = adjustedGroupDelay_2 + max_delay_2
...
组G: adjustedGroupDelay_G = adjustedGroupDelay_{G-1} + max_delay_{G-1}
```

**总调度时间**：

```
Total_Delay = base_time + Σ(max_delay_i) for i=1 to G-1
```

## 三、AUV 过度调度问题分析

### 3.1 什么是 AUV 过度调度？

**定义**：当 AUV 请求数量过多，或 AUV 请求被过度优先处理，导致：

1. AUV 请求占用大量时间槽
2. 普通节点请求被延迟
3. 整体网络吞吐量下降

### 3.2 过度调度的场景

#### 场景 1：AUV 请求数量过多

假设：

- 总请求数：30 个
- AUV 请求数：20 个（占 67%）
- 普通请求数：10 个（占 33%）

**问题**：

- 如果 AUV 请求都被分配到前几个组
- 普通请求被分配到后面的组
- 普通请求的延迟会显著增加

#### 场景 2：AUV 请求被过度优先

即使 AUV 请求数量不多，但如果：

- 所有 AUV 请求都被分配到组 1（最小组号）
- 普通请求被分配到组 2、3、4...
- 组 1 的延迟可能很大（如果组内成员多）

### 3.3 代码中的潜在问题

#### 3.3.1 优先级排序可能导致过度集中

```cpp
// AUV请求优先排序
if (a_is_auv != b_is_auv) return a_is_auv;  // AUV请求优先
return sectors[a].distance < sectors[b].distance;  // 距离小的优先
```

**问题**：

- 所有 AUV 请求都会优先分配
- 如果 AUV 请求数量多，可能都分配到前几个组
- 导致前几个组过大，延迟增加

#### 3.3.2 能量函数奖励可能不够

```cpp
if (group_sizes[solution[i]] <= 3) {
    energy -= 0.5;  // 奖励较小
}
```

**问题**：

- 奖励值（0.5）相对于冲突惩罚（100.0）和组数惩罚（1.0）较小
- 在模拟退火过程中，可能被其他因素覆盖
- 无法有效保证 AUV 请求分配到小组

## 四、普通节点延迟分析

### 4.1 延迟计算公式

对于普通节点请求，其延迟包括：

1. **等待调度时间**：从发送 RTS 到收到 CTS 的时间
2. **组间延迟**：前面所有组的最大延迟之和
3. **组内延迟**：所在组内的最大延迟
4. **传输延迟**：数据包传输时间

**总延迟**：

```
Delay_普通节点 = T_调度 + Σ(max_delay_i) + max_delay_所在组 + T_传输
```

其中：

- `T_调度`：调度算法执行时间（通常很小，可忽略）
- `Σ(max_delay_i)`：前面所有组的最大延迟之和
- `max_delay_所在组`：所在组的最大延迟
- `T_传输`：数据包传输时间

### 4.2 延迟分析示例

#### 示例 1：正常情况（AUV 请求 30%，普通请求 70%）

假设：

- 总请求数：30 个
- AUV 请求：9 个（30%）
- 普通请求：21 个（70%）
- 分组结果：5 个组

**理想分组**（AUV 请求优先但不集中）：

- 组 1：3 个 AUV + 2 个普通，max_delay = 0.5s
- 组 2：3 个 AUV + 2 个普通，max_delay = 0.5s
- 组 3：3 个 AUV + 2 个普通，max_delay = 0.5s
- 组 4：7 个普通，max_delay = 0.8s
- 组 5：8 个普通，max_delay = 0.9s

**延迟计算**：

- 组 1 的普通节点：0.5s
- 组 2 的普通节点：0.5s + 0.5s = 1.0s
- 组 3 的普通节点：0.5s + 0.5s + 0.5s = 1.5s
- 组 4 的普通节点：0.5s + 0.5s + 0.5s + 0.8s = 2.3s
- 组 5 的普通节点：0.5s + 0.5s + 0.5s + 0.8s + 0.9s = 3.2s

**平均延迟**：约 1.8s

#### 示例 2：AUV 过度调度（AUV 请求 70%，普通请求 30%）

假设：

- 总请求数：30 个
- AUV 请求：21 个（70%）
- 普通请求：9 个（30%）
- 分组结果：5 个组

**过度集中分组**（所有 AUV 请求在前）：

- 组 1：7 个 AUV，max_delay = 1.2s
- 组 2：7 个 AUV，max_delay = 1.2s
- 组 3：7 个 AUV，max_delay = 1.2s
- 组 4：3 个普通，max_delay = 0.4s
- 组 5：6 个普通，max_delay = 0.7s

**延迟计算**：

- 组 1 的 AUV 节点：1.2s
- 组 2 的 AUV 节点：1.2s + 1.2s = 2.4s
- 组 3 的 AUV 节点：1.2s + 1.2s + 1.2s = 3.6s
- 组 4 的普通节点：1.2s + 1.2s + 1.2s + 0.4s = **4.0s** ⚠️
- 组 5 的普通节点：1.2s + 1.2s + 1.2s + 0.4s + 0.7s = **4.7s** ⚠️

**平均延迟**：约 3.2s（比正常情况增加 78%）

### 4.3 延迟过大的判断标准

根据时间槽结构：

- `slotSum = 20s`：一个完整的时间槽
- 如果总延迟接近或超过 `slotSum`，可能导致：
  - 数据包无法在当前时间槽内完成传输
  - 需要延迟到下一个时间槽
  - 进一步增加延迟

**判断标准**：

- **可接受**：总延迟 < 15s（75% 的时间槽）
- **警告**：总延迟 15s - 18s（75% - 90% 的时间槽）
- **严重**：总延迟 > 18s（> 90% 的时间槽）

## 五、问题根源分析

### 5.1 优先级机制的问题

#### 问题 1：优先级过于绝对

```cpp
if (a_is_auv != b_is_auv) return a_is_auv;  // 绝对优先
```

**问题**：

- AUV 请求总是优先于普通请求
- 没有考虑 AUV 请求的数量和分布
- 可能导致 AUV 请求过度集中

#### 问题 2：能量函数奖励不足

```cpp
if (group_sizes[solution[i]] <= 3) {
    energy -= 0.5;  // 奖励值太小
}
```

**问题**：

- 奖励值（0.5）相对于其他惩罚项太小
- 在模拟退火过程中可能被忽略
- 无法有效控制 AUV 请求的分布

### 5.2 调度机制的问题

#### 问题 1：组间延迟累加

```cpp
adjustedGroupDelay = adjustedGroupDelay + groupInMaxDelay;
```

**问题**：

- 如果前面的组延迟大，后面的组延迟会累积
- 没有考虑公平性
- 普通节点可能受到严重影响

#### 问题 2：没有延迟上限

代码中虽然有检查：

```cpp
if(adjustedGroupDelay > groupBegin + slotTime1 + slotTime2) {
    // 延迟到下一轮
}
```

但这是被动检查，不是主动控制。

## 六、解决方案建议

### 6.1 改进优先级机制

#### 方案 1：相对优先级

```cpp
// 不是绝对优先，而是加权优先
double a_priority = (a_is_auv ? 2.0 : 1.0) * (1.0 / sectors[a].distance);
double b_priority = (b_is_auv ? 2.0 : 1.0) * (1.0 / sectors[b].distance);
return a_priority > b_priority;
```

**优点**：

- AUV 请求有优势，但不是绝对优先
- 考虑距离因素
- 更平衡的分配

#### 方案 2：限制 AUV 请求集中度

```cpp
// 限制每个组中 AUV 请求的最大数量
const int MAX_AUV_PER_GROUP = 3;

// 在分配时检查
if (is_auv_request && group_auv_count[g] >= MAX_AUV_PER_GROUP) {
    continue;  // 跳过这个组
}
```

**优点**：

- 防止 AUV 请求过度集中
- 保证普通请求也有机会分配到前面的组
- 更公平的调度

### 6.2 改进能量函数

#### 方案 1：增加 AUV 奖励

```cpp
// 增加奖励值
if (group_sizes[solution[i]] <= 3) {
    energy -= 2.0;  // 从 0.5 增加到 2.0
}

// 或者根据组大小动态调整
if (group_sizes[solution[i]] == 1) {
    energy -= 3.0;  // 单独成组，最大奖励
} else if (group_sizes[solution[i]] == 2) {
    energy -= 2.0;
} else if (group_sizes[solution[i]] == 3) {
    energy -= 1.0;
}
```

#### 方案 2：惩罚 AUV 请求过度集中

```cpp
// 惩罚：如果 AUV 请求过度集中
int auv_in_group = 0;
for (int j = 0; j < n; ++j) {
    if (solution[j] == solution[i] && sectors[j].destID == auvId) {
        auv_in_group++;
    }
}
if (auv_in_group > 3) {
    energy += 1.0;  // 惩罚过度集中
}
```

### 6.3 改进调度机制

#### 方案 1：延迟上限控制

```cpp
const simtime_t MAX_GROUP_DELAY = 15s;  // 最大组延迟

// 在分配时检查
if (estimated_delay > MAX_GROUP_DELAY) {
    // 拒绝分配，或分配到下一轮
    continue;
}
```

#### 方案 2：公平性调度

```cpp
// 轮询调度：AUV 请求和普通请求交替分配
bool use_auv = true;
for (int idx : sector_order) {
    if (is_auv_request[idx] != use_auv) {
        continue;  // 跳过不符合当前轮次的请求
    }
    // 分配逻辑
    use_auv = !use_auv;  // 切换
}
```

### 6.4 监控和预警机制

```cpp
// 计算当前调度的总延迟
simtime_t total_delay = 0;
for (const auto& group : sectorGroups) {
    total_delay += groupInMaxDelay;
}

// 预警
if (total_delay > 0.9 * slotSum) {
    EV_WARN << "警告：总延迟超过时间槽的 90%，普通节点可能延迟过大" << endl;
}

// 统计 AUV 请求占比
int auv_count = 0;
for (const auto& sector : sectors) {
    if (sector.destID == auvId) auv_count++;
}
double auv_ratio = static_cast<double>(auv_count) / sectors.size();

if (auv_ratio > 0.5) {
    EV_WARN << "警告：AUV 请求占比超过 50%，可能存在过度调度风险" << endl;
}
```

## 七、总结

### 7.1 AUV 优先级的物理意义

1. **时间敏感性**：AUV 通信窗口有限，必须优先服务
2. **数据时效性**：AUV 数据收集任务有时间要求
3. **网络效率**：优先服务 AUV 可以提高整体网络效率

### 7.2 AUV 过度调度的问题

1. **AUV 请求过度集中**：导致前几个组延迟大
2. **普通节点延迟累积**：后面组的延迟会累积前面所有组的延迟
3. **时间槽利用率低**：如果总延迟接近或超过时间槽长度，效率下降

### 7.3 是否会导致普通节点延迟过大？

**答案：可能会，取决于以下因素**：

1. **AUV 请求占比**：

   - < 30%：影响较小
   - 30% - 50%：中等影响
   - > 50%：**严重影响** ⚠️

2. **AUV 请求分布**：

   - 均匀分布：影响较小
   - 集中在前几个组：**影响严重** ⚠️

3. **组内延迟**：
   - 如果 AUV 请求所在组延迟大，会影响后续所有组

### 7.4 建议

1. **实施相对优先级**：不是绝对优先，而是加权优先
2. **限制 AUV 请求集中度**：每个组最多 3 个 AUV 请求
3. **增加能量函数奖励**：提高 AUV 请求分配到小组的奖励
4. **实施延迟上限**：防止总延迟过大
5. **添加监控机制**：实时监控延迟和 AUV 请求占比

### 7.5 关键指标

- **AUV 请求占比**：应 < 50%
- **总调度延迟**：应 < 15s（75% 的时间槽）
- **普通节点平均延迟**：应 < 2s
- **组内 AUV 请求数**：应 ≤ 3 个

