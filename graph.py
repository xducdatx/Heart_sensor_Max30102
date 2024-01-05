import serial
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('COM3', 115200)
sample_size = 150
data1 = []
data2 = []
data3 = [0] * sample_size
value2 = 0
count = 0
temp = 0
# Tạo đồ thị và đường đồ thị
plt.ion()
fig, ax = plt.subplots(figsize=(16, 6))
line1, = ax.plot(data1, label='Line 1')
line2, = ax.plot(data2, label='Line 2', linestyle='--')  # Đường mới với linestyle '--'

# Đặt chú thích cho đường đồ thị
ax.legend()

try:
    while True:
        value = ser.readline().decode().strip()
        
        if value:
            value = float(value)
            data3[temp % sample_size] = value
            temp = temp + 1
            value2 = sum(data3) 
            if count < sample_size:
                count = count + 1

            data1.append(value)
            data2.append(value2/count)  # Đặt giá trị của đường mới bằng giá trị của đường ban đầu
            # Cập nhật đường đồ thị
            line1.set_xdata(np.arange(len(data1)))
            line1.set_ydata(data1)
            
            line2.set_xdata(np.arange(len(data2)))
            line2.set_ydata(data2)
            
            # Cập nhật giới hạn của trục y và trục x
            ax.relim()
            ax.autoscale_view()
            
            # Vẽ lại đồ thị
            fig.canvas.draw()
            fig.canvas.flush_events()

except KeyboardInterrupt:
    ser.close()
finally:
    # Hiển thị cửa sổ đồ thị fullscreen
    mng = plt.get_current_fig_manager()
    mng.full_screen_toggle()
    plt.show()