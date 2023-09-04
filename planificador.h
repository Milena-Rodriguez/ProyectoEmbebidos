#include <stdbool.h>
#include <stdlib.h>

/* Número máximo de tareas que se pueden planificar */
#define MAX_TASKS 10

/* Tipo de datos tiempo */
typedef unsigned long tTime;

/* Definición callback tarea */
typedef void (*TaskCallback)(void *);

/* Estructura tarea */
struct Task {
  tTime runNextAt;        //La próxima vez (en milisegundos) en la cual la tarea debe ser ejecutada
  tTime timeBetweenRuns;	//Periodo en milisegundos si la tarea es periodica.
  TaskCallback callback;	//Función callback de la tarea
  bool enabled;			//Estado de la tarea (habilitada/deshabilitada)
  bool valid;           //Tarea existe
};

/**
   Estimación del transcurso del tiempo en milisegundos.

   @return la cantidad de milisegundos desde el inicio del sistema. -1 si hubo error.
*/
tTime TimeNow();

/**
   Estimación del transcurso del tiempo en milisegundos desde un punto
   específico.

   @param since Punto específico de tiempo.

   @return la cantidad de milisegundos transcurridos desde since.
*/
tTime TimePassed(tTime since);

/**
  Agrega una tarea al planificador. La tarea es agreagada al arreglo interno _scheduled_tasks.
  Por defecto, la tarea es creada en estado deshabilitada.

  @param now Tiempo actual del sistema
  @param timeFromNow Tiempo en milisegundos dentro del cual la tarea debe ser planificada. Usar 0 para especificar que la tarea sea ejecutada inmediatamente.
  @param timeBetweenRuns Periodo de planificación si la tarea es periodica. Usar 0 para tareas no periodicas.
  @param callback Puntero a función "callback" que debe ejecutarse.

  @return Puntero a nueva tarea en el arreglo. NULL si hubó un error.
*/
struct Task *SchedulerAddTask(tTime now, tTime timeFromNow, tTime timeBetweenRuns, TaskCallback callback);

/**
   Ejecuta el planificador, busca en el arreglo interno _scheduled_tasks la próxima tarea "enabled" que debe ser planificada.
   Esta función debe ejecutarse en cadad "system tick".
*/
void SchedulerRun();

/**
   Planifica la tarea t para que se ejecute luego de timeFromNow milisegundos.

   @param t Puntero a a la tarea.
   @param timeFromNow Tiempo en milisegundos dentro del cual la tarea debe ser planificada.

*/
void TaskSetNextTime(struct Task *t, tTime timeFromNow);

/**
   Resetea la tarea periodica t para que sea planificada en el siguiente periodo.

   @param t Puntero a a la tarea.

*/
void TaskResetPeriodic(struct Task *t);

/**
   Habilita/Deshabilita la tarea t.

   @param t Puntero a a la tarea.

*/
void TaskEnable(struct Task* t);
void TaskDisable(struct Task* t);