from eps_driver_SERGEI import EPS_driver
import time
from datetime import datetime




wait_time = 0.5

EPS = EPS_driver()
EPS.set_comms_watchdog_period(90)
time.sleep(3)
EPS.get_comms_watchdog_period()


print("***12 VOLT BUS***")
EPS.Pow_12v()
time.sleep(wait_time)
print("***5 VOLT BUS***")
EPS.Pow_5v()
time.sleep(wait_time)
print("***3.3 VOLT BUS***")
EPS.Pow_3v3()
time.sleep(wait_time)

for i in range(10):
    print('CURRENT ON SWITCH 8:')
    EPS.current_switch_8()
    EPS.voltage_switch_8()
    time.sleep(1)

EPS.get_initial_state_of_all_PDMs()
time.sleep(1)
EPS.get_expected_state_of_all_PDMs()
time.sleep(1)
for i in range(1, 11):
    EPS.get_PDM_N_actual_status(i)
time.sleep(1)


EPS.switch_PDM_N_off(8)
print('WE HAVE JUST SWITCHED PDM #8 OFF')
time.sleep(3)
EPS.get_PDM_N_actual_status(8)


EPS.get_initial_state_of_all_PDMs()
time.sleep(1)
EPS.get_expected_state_of_all_PDMs()
time.sleep(1)
for i in range(1, 11):
    EPS.get_PDM_N_actual_status(i)
time.sleep(1)





EPS.switch_PDM_N_on(8)
print('WE HAVE JUST SWITCHED PDM #8 ON')
time.sleep(3)
EPS.get_PDM_N_actual_status(8)


# EPS.get_initial_state_of_all_PDMs()
# time.sleep(1)
# EPS.get_expected_state_of_all_PDMs()
# time.sleep(1)
# for i in range(1, 11):
#     EPS.get_PDM_N_actual_status(i)
#
#
#
# for i in range(10):
#     print('CURRENT ON SWITCH 8:')
#     EPS.current_switch_8()
#     EPS.voltage_switch_8()
#     time.sleep(1)
