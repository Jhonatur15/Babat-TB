import xlwt
import serial
from xlwt import Workbook

wb = Workbook()

sheet1 = wb.add_sheet("Sheet 1")

# Menambahkan definisi di baris pertama
sheet1.write(0, 0, "Berat Obat")
sheet1.write(0, 2, "Jumlah Obat")

ser = serial.Serial('COM10', 9600, timeout=1)
ser.flush()
count = 0
jumlah_obat = 0

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()
        print(line)
        sheet1.write(count + 1, 0, line)

        value = float(line)

        # Hitung jumlah obat berdasarkan perbandingan berat dengan 1.57
        jumlah_obat = int(value / 1.57)

        sheet1.write(count + 1, 2, jumlah_obat)  # Menyimpan jumlah obat ke kolom 2

        wb.save("babat_bobot_bebet.xls")
        count += 1

        print(f"Jumlah Obat: {jumlah_obat}")
