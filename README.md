# Título do Projeto 

**Smart Ohmmeter - Ohmímetro inteligente**

## Objetivo Geral 

O objetivo do projeto é projetar um sistema ohmímetro capaz de medir resistores da **série E24**, e apresentar graficamente o código de cores referente ao resistor que foi medido.

## Descrição Funcional 

O projeto conta com um divisor de tensão capaz de medir a tensão utilizando o **conversor AD** da **BitDoglab**, sendo possível, assim, medir a resistência de um resistor desconhecido, fixando um resistor de referência. Além disso, o valor medido é aproximado para o valor de resistência mais próximo contido na **série E24**, para ser possível apresentar o código de cores.

O código de cores é apresentado em um display gráfico monocromático, utilizando de textos que referenciam as cores, tornando o visual de fácil entendimento.

## Descreva os pontos mais relevantes tanto do dos Periféricos da **BitDogLab**/**RP2040** quanto do  seu código. 

Neste projeto, foi necessário utilizar a **BitDogLab** com alguns **periféricos** e **resistores**.

Os **resistores** foram utilizados para montar um divisor de tensão para que fosse capaz de medir a tensão em cima de um resistor específico. Dois resistores foram utilizados: um conhecido, de referência; e um segundo desconhecido, cujo valor deve ser calculado, usando a funcionalidade final do projeto.

O resistor de referência utilizado foi de **10.000 ohms**, sendo bastante preciso na medição, até valores de **22.000 ohms**. Acima deste valor, a precisão fica baixa, sendo necessário alterar o resistor de referência, aumentando seu valor. Quanto mais próximo o valor do resistor de referência é do resistor desconhecido, melhor a sua precisão.

Já sobre os periféricos utilizados, temos o **display SSD1306 OLED** para apresentação de resultados, onde é apresentado o valor lido de resistência, além também de um desenho para representar o resistor e seu código de cores, em palavras abreviadas. As palavras são:

- **Pr:** Preto;
- **Mr:** Marrom;
- **Vm:** Vermelho;
- **Lr:** Laranja;
- **Am:** Amarelo;
- **Vr:** Verde;
- **Az:** Azul;
- **Vl:** Violeta;
- **Cz:** Cinza;
- **Bn:** Branco.

Outro periférico utilizado foi o **botão A**, utilizado para a interrupção externa. A interrupção altera a visualização presente no display, mostrando ora a medição do resistor, ora seu código de cores.

Seguindo com os periféricos, temos o **conversor AD**, utilizado para converter as leituras analógicas do divisor de tensão em leituras digitais, sendo possível medir a tensão, e assim, calcular a resistências desconhecida. Para isso, foi feito uma captura de **500 amostras** durante meio segundo, para aumentar a precisão da leitura, e tirado uma média. Logo após isso, a conversão em tensão, e depois o cálculo para encontrar a resistência é feito.

Por fim, um ponto importante na lógica do código foi a função de aproximação dos valores para valores conhecidos da **série E24** de resistências. Essa função foi feita utilizando logaritmo e potência, sendo possível capturar os dígitos com maior facilidade, e eliminando o uso de if else desnecessário. Com o logaritmo é possível capturar a ordem do número que foi medido, e com a potenciação, foi possível normalizar os valores para valores padrões, facilitando o cálculo.

Finalizando, uma melhoria possível - e necessária - no projeto, é a adição de um “switch” capaz de alterar a resistência de referência conforme os valores vão aumentando, para a precisão ser a maior possível dentro da medição. Outro ponto observado foi que, utilizando um multímetro digital, foi possível observar que os cálculos diferenciam um pouco da medição absoluta, o que é normal, visto que a precisão do multímetro é vezes maior.

## Links para acesso ao código e ao vídeo.

Clique em ***[link do video](https://drive.google.com/file/d/1OK2isNOO2DdbdRPEB4QoKdVR_TIcRxZw/view?usp=sharing)*** para visualizar o vídeo ensaio do projeto.