import serial
import re
#sudo chmod a+rw /dev/ttyACM0

port = '/dev/ttyACM0'  # Substitua 'COMX' pela porta serial do Arduino

# Abre a porta serial
with serial.Serial(port, 9600) as ser:
    while True:
        line = ser.readline().decode(errors='ignore').strip()

        if line == "No finger detected":
            continue  # Pula para a próxima iteração do loop

        # Procura por um padrão de ID na linha
        match = re.search(r'Found ID #(\d+) with confidence', line)
        if match:
            id = match.group(1)  # Obtém o ID correspondente

            # Imprime a mensagem desejada
            print(f"Chamada registrada para o aluno com o ID: {id}")
