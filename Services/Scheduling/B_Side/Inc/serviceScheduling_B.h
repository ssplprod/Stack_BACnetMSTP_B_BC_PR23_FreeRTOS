
#ifdef BACDEL_OBJ_SDL
#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "objDevice.h"

/** structure to collect schedule events */
typedef struct ActiveExcepTimeVal
{
    BACnetTimeValue_t       m_stListOfTimeValues;
    /* Event Priority (1..16) */
    uint32_t                m_u32EventPriority;
    struct ActiveExcepTimeVal *m_pstNext;
}ActiveExcepTimeVal_t;

/** functions */
void Get_PresentValue_ForSchedule(ulong32_t ul32BaseAddr,void *pInData,int8_t i8ArrayIndex, void *pVirtualDev, bool bChangeReliability);
void SetSDLPresentValWeekly(AnyValue_t *pstSHDLPresentValue,BACnetTimeValue_t *pstTimeValue,ulong32_t ul32BaseAddr,
							 void *pVirtualDev);

bool SetSDLPresentValExcp(  AnyValue_t  *pstSHDLPresentValue,
                        ListOfSpecialEvent_t  *m_pstSplEvent,
                        ulong32_t   ul32BaseAddr,
                        void *pVirtualDev);
void SetPresentValueWithTag(BACNET_PROPERTY_VALUE   *pstPresentValue, BACNET_PROPERTY_VALUE m_stPropValue);

BACNET_RETURN_TYPE SchedulingInternal(BACnetDevObjPropRef_t  *pstDevObjPropRef,
                        virtualDevData_t *pVirtualDev,
                        AnyValue_t   *pstSHDLPresentValue,
                        uint32_t              i32Priority);

bool SchedulingExternal(BACnetDevObjPropRef_t  *pstDevObjPropRef, bacnetip_arguments_t *pstServiceArgs,
                        AnyValue_t *pstSHDLPresentValue, uint32_t   i32Priority, void *pVirtualDev);
bool IS_ConfigurationError(ListOfBACnetDevObjPropRef_t *pstListOfBACnetDevObjPropReff,ListOfSpecialEvent_t  *pstSplEvent,
                           Pr_ListOfBACnetDailySchedule_t  *pstWeeklySchedule,AnyValue_t *pstScheduleDefault,void *pVirtualDev);
ActiveExcepTimeVal_t *Get_Excpt_Event_InEffect(ListOfSpecialEvent_t    *pstSplEvent,
                            BACnetTimeValue_t *stListOfTimeValues,
                            void *pVirtualDev);

ActiveExcepTimeVal_t *FillActiveExcepTimeVal(BACnetTimeValue_t *pstListOfTimeValues,
                                             uint32_t  u32EventPriority,
                                             BACnetTime_t  stSyetemTime);
BACnetTime_t GetNextTriggerTimeExcp(ActiveExcepTimeVal_t *pstListofActExcepTimeVal,BACnetTime_t stSyetemTime);
BACnetTime_t GetNextTriggerTimeWeekly(BACnetTimeValue_t   *pstListOfTimeValues,BACnetTime_t stSyetemTime);
TIMER_CALLBACK SCHEDULE_TIMER(void* lpParam, uint8_t TimerOrWaitFired);

#ifdef BACDEL_OBJ_CAL
/** function to trigger present value of calendar object whenever device
	date_time changes due to time_sync or utc_time_sync */
void Trigger_Calendar_Object(virtualDevData_t *pVirtualDev);
#endif

/** function to trigger present value of schedule object whenever device
	date_time changes due to time_sync or utc_time_sync */
void Trigger_Schedule_Object(virtualDevData_t *pVirtualDev);

#endif /* SCHEDULING_H */
#endif
