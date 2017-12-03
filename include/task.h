#pragma once

struct kmqTask
{
    uint32_t task_id;

    // public interface
    int (*del)(struct kmqTask *self);
};

int kmqTask_new(struct kmqTask **task);

