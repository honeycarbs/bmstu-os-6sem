#include <linux/init.h>       // макросы __init, __exit
#include <linux/init_task.h>  // init_task
#include <linux/kernel.h>  //уровни протоколирования, printk
#include <linux/module.h>  // макросы module_init, module_exit, MODULE_LICENSE ...
#include <linux/sched.h>  // task_struct

//сообщить ядру под какой лицензией распространяется исходный код модуля,
//что влияет на то, к каким функциям и переменным может получить доступ
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kazaeva Tatiana");

// __init - функция используется только на этапе инициализации и освобождает
// ресурсы после

// двойное подчеркивание указывает что данная функция является низкоуровневой
static int __init mod_init(void) {
    struct task_struct *task = &init_task; // структура - дескриптор процесса
    printk(KERN_INFO " > module is loaded.\n");
    do {
      printk(KERN_INFO " > %s - %d, parent %s - %d", // позволяет отправлять сообщения в системный журнал.
      task->comm, task->pid, task->parent->comm, task->parent->pid); // KERN_INFO - уровень приоритета сообщения (информационный)
    } while ((task = next_task(task)) != &init_task);

    // символ current определяет текущий процесс
    printk(KERN_INFO " > %s - %d, parent %s - %d", 
           current->comm, current->pid, current->parent->comm, current->parent->pid);
    return 0;
}

static void __exit mod_exit(void) {
  printk(KERN_INFO " > module unloaded.\n");
}

// регистрация функции инициализации модуля - запрос ресурсов и выделение памяти
// под структуры
module_init(mod_init);
// регистрация функции которая вызывается при удалении модуля из
// ядра - освобождение ресурсов - после завершения функции модуль выгружается
module_exit(mod_exit);
