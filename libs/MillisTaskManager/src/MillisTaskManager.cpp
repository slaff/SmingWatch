/*
 * MIT License
 * Copyright (c) 2018-2020 _VIFEXTech
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "MillisTaskManager.h"

#define TASK_NEW(task) do{task = new Task_t;}while(0)
#define TASK_DEL(task) do{delete task;}while(0)

/**
  * @brief  调度器析构，释放任务链表内存
  * @param  无
  * @retval 无
  */
MillisTaskManager::~MillisTaskManager()
{
    /*移动到链表头*/
    Task_t* now = head;
    while(true)
    {
        /*当前节点是否为空*/
        if(now == nullptr)
            break;

        /*将当前节点缓存，等待删除*/
        Task_t* now_del = now;

        /*移动到下一个节点*/
        now = now->next;

        /*删除当前节点内存*/
        TASK_DEL(now_del);
    }
}

/**
  * @brief  往任务链表添加一个任务，设定间隔执行时间
  * @param  func:任务函数指针
  * @param  timeMs:周期时间设定(毫秒)
  * @param  state:任务开关
  * @retval 任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::add(TaskFunction_t func, uint32_t timeMs, bool active)
{
    /*寻找当前函数*/
    Task_t* task = find(func);
    
    /*如果被注册*/
    if(task != nullptr)
    {
        /*更新信息*/
        task->time = timeMs;
        task->active = active;
        return task;
    }

    /*为新任务申请内存*/
    TASK_NEW(task);

    /*是否申请成功*/
    if(task == nullptr)
    {
        return nullptr;
    }

    task->function = func;        //任务回调函数
    task->time = timeMs;          //任务执行周期
    task->active = active;          //任务状态
    task->timePrev = 0;           //上一次时间
    task->timeCost = 0;           //时间开销
    task->timeError = 0;          //误差时间
    task->next = nullptr;            //下一个节点
    
    /*如果任务链表为空*/
    if(head == nullptr)
    {
        /*将当前任务作为链表的头*/
        head = task;
    }
    else
    {
        /*从任务链表尾部添加任务*/
        tail->next = task;
    }
    
    /*将当前任务作为链表的尾*/
    tail = task;
    return task;
}

/**
  * @brief  寻找任务,返回任务节点
  * @param  func:任务函数指针
  * @retval 任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::find(TaskFunction_t func)
{
    Task_t* now = head;
    Task_t* task = nullptr;
    while(true)
    {
        if(now == nullptr)//当前节点是否为空
            break;

        if(now->function == func)//判断函数地址是否相等
        {
            task = now;
            break;
        }

        now = now->next;//移动到下一个节点
    }
    return task;
}

/**
  * @brief  获取当前节点的前一个节点
  * @param  task:当前任务节点地址
  * @retval 前一个任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::getPrev(Task_t* task)
{
    Task_t* now = head;    //当前节点
    Task_t* prev = nullptr;   //前一节点
    Task_t* retval = nullptr; //被返回的节点
    
    /*开始遍历链表*/
    while(true)
    {
        /*如果当前节点为空*/
        if(now == nullptr)
        {
            /*遍历结束*/
            break;
        }
        
        /*如果当前节点与被搜索的节点匹配*/
        if(now == task)
        {
            /*保存前一个节点，准备返回*/
            retval = prev;
            break;
        }
        
        /*当前节点保存为前一节点*/
        prev = now;
        
        /*节点后移*/
        now = now->next;
    }
    return retval;
}

/**
* @brief  注销任务（谨慎使用，线程不安全）
  * @param  func:任务函数指针
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::remove(TaskFunction_t func)
{
    Task_t* task = find(func);
    if(task == nullptr)
        return false;

    Task_t* prev = getPrev(task); //前一个节点
    Task_t* next = task->next;    //后一个节点
    
    /*如果被删除节点在链表头*/
    if(prev == nullptr && next != nullptr)
    {
        /*将后一个节点作为链表头*/
        head = next;
    }
    /*如果被删除节点在链表尾*/
    else if(prev != nullptr && next == nullptr)
    {
        /*将前一个节点作为链表尾*/
        prev->next = nullptr;
    }
    /*如果被删除节点在链表中间*/
    else if(prev != nullptr && next != nullptr)
    {
        /*将前一个节点对接至后一个节点*/
        prev->next = next;
    }
    
    /*删除当前节点*/
    TASK_DEL(task);
    
    return true;
}

/**
  * @brief  任务状态控制
  * @param  func:任务函数指针
  * @param  state:任务状态
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::setState(TaskFunction_t func, bool active)
{
    Task_t* task = find(func);
    if(task == nullptr)
        return false;

    task->active = active;
    return true;
}

/**
  * @brief  任务执行周期设置
  * @param  func:任务函数指针
  * @param  timeMs:任务执行周期
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::setIntervalTime(TaskFunction_t func, uint32_t timeMs)
{
    Task_t* task = find(func);
    if(task == nullptr)
        return false;

    task->time = timeMs;
    return true;
}

#if (MTM_USE_CPU_USAGE == 1)
#include <Arduino.h>                //需要使用micros()
static uint32_t userFuncLoopUs = 0; //累计时间
/**
  * @brief  获取CPU占用率
  * @param  无
  * @retval CPU占用率，0~100%
  */
float MillisTaskManager::getCpuUsage()
{
    static uint32_t mtmStartUs;
    float usage = (float)userFuncLoopUs / (micros() - mtmStartUs) * 100.0f;

    if(usage > 100.0f)
        usage = 100.0f;

    mtmStartUs = micros();
    userFuncLoopUs = 0;
    return usage;
}
#endif

/**
  * @brief  时间差判定
  * @param  nowTick:当前时间
  * @param  prevTick:上一个时间
  * @retval 时间差
  */
uint32_t MillisTaskManager::getTickElaps(uint32_t nowTick, uint32_t prevTick)
{
    uint32_t actTime = nowTick;

    if(actTime >= prevTick)
    {
        prevTick = actTime - prevTick;
    }
    else
    {
        prevTick = /*UINT32_MAX*/0xFFFFFFFF - prevTick + 1;
        prevTick += actTime;
    }

    return prevTick;
}

/**
  * @brief  获取任务单次耗费时间(us)
  * @param  func:任务函数指针
  * @retval 任务单次耗费时间(us)
  */
uint32_t MillisTaskManager::getTimeCost(TaskFunction_t func)
{
    Task_t* task = find(func);
    if(task == nullptr)
        return 0;

    return task->timeCost;
}

/**
  * @brief  调度器(内核)
  * @param  tick:提供一个精确到毫秒的系统时钟变量
  * @retval 无
  */
void MillisTaskManager::running(uint32_t tick)
{
    Task_t* current = head;
    while(true)
    {
        /*当前节点是否为空*/
        if(current == nullptr)
        {
            /*遍历结束*/
            break;
        }

        if(current->function != nullptr && current->active)
        {
            uint32_t elapsTime = getTickElaps(tick, current->timePrev);
            if(elapsTime >= current->time)
            {
                /*获取时间误差，误差越大实时性越差*/
                current->timeError = elapsTime - current->time;
                
                /*记录时间点*/
                current->timePrev = tick;

#if (MTM_USE_CPU_USAGE == 1)
                /*记录开始时间*/
                uint32_t start = micros();
                
                /*执行任务*/
                current->function();
                
                /*获取执行时间*/
                uint32_t timeCost = micros() - start;
                
                /*记录执行时间*/
                current->timeCost = timeCost;
                
                /*总时间累加*/
                userFuncLoopUs += timeCost;
#else
                current->function();
#endif
                
                /*判断是否开启优先级*/
                if(priorityEnable)
                {
                    /*遍历结束*/
                    break;
                }
            }
        }

        /*移动到下一个节点*/
        current = current->next;
    }
}
