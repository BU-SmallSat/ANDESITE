from eps_driver import EPS_driver
import time
import datetime


'''THIS IS THE CODE FOR TESTING SMALL PARTS OF THE EPS TELEMETRY CODE, SUCH AS SEPARATE FUNCTIONS'''
'''STILL HAVE TO BE TESTED:
PCM_reset
manual_reset
'''

wait_time = 0.5

EPS = EPS_driver()

print("***12 VOLT BUS***")
EPS.Pow_12v()
time.sleep(wait_time)
print("***5 VOLT BUS***")
EPS.Pow_5v()
time.sleep(wait_time)
print("***3.3 VOLT BUS***")
EPS.Pow_3v3()
time.sleep(wait_time)

EPS.set_PDM_N_initial_state_off(1)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(2)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(3)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(4)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(5)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(6)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(7)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(8)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(9)
time.sleep(.5)
EPS.set_PDM_N_initial_state_off(10)
time.sleep(.5)

EPS.get_initial_state_of_all_PDMs()

EPS.switch_off_all_PDMs()
print('WE SWITCHED OFF ALL PDMs')
time.sleep(5)
EPS.set_PDM_N_timer_limit(0xFF, 8)
time.sleep(1)
EPS.get_expected_state_of_all_PDMs()
time.sleep(1)
for i in range(1, 11):
    EPS.get_PDM_N_actual_status(i)


time.sleep(5)
print('NOW WE ARE SWITCHING #8 ON\n\n\n')
EPS.switch_PDM_N_on(8)
time.sleep(5)
EPS.get_expected_state_of_all_PDMs()
time.sleep(1)
for i in range(1, 11):
    EPS.get_PDM_N_actual_status(i)



EPS.get_PDM_N_timer_limit(8)







'''
EPS.switch_off_all_PDMs()
print('WE SWITCHED OFF ALL PDMs')
time.sleep(5)
# EPS.switch_PDM_N_on(1)
# EPS.switch_PDM_N_on(2)
# # EPS.get_PDM_N_tcurrent_timer_value(1)
# # time.sleep(30)
# # EPS.get_PDM_N_tcurrent_timer_value(1)
# # time.sleep(30)
# # EPS.get_PDM_N_tcurrent_timer_value(1)
# # time.sleep(30)
#
#
EPS.get_expected_state_of_all_PDMs()
for i in range(1, 11):
    EPS.get_PDM_N_actual_status(i)
#
# EPS.get_PDM_N_actual_status(2)
# EPS.get_PDM_N_actual_status(3)
# # # EPS.set_all_PDMs_to_initial_state()
# # # time.sleep(5)
# EPS.get_expected_state_of_all_PDMs()
# EPS.get_actual_state_of_all_PDMs()
#
# # EPS.switch_PDM_N_off(2)
# # EPS.get_expected_state_of_all_PDMs()
#
# EPS.voltage_switch_1()
# EPS.current_switch_1()
'''

'''TESTING ON JUNE, 3'''












print('END OF THE TESTING SCRIPT')