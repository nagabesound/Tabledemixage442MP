# Table de mixage 442MP
Traitement audio en temps reél via un STM32

## Objectif:
échantilloner un signal analogique, le traité et sortir le signal traité sur le haut-parleur. 

## Principe: 
L'acquisition se fait via DMA avec un double buffer sur l'ADC1. On place ensuite les échantillons dans un deuxieme doucle buffer plus grand pour le traitement et on envoie au DAC les échantillons traité via DMA et double buffer. 
Avec deux potentiometres tactiles on peut controler le volume d'entré dans le buffer de traitement et le volume de sortie. 

On a la possibilité d'affiche quand le signal detecte un certain seuil ou quand il sature en caculant la valeur efficace du buffer de traitement. Cette option marcherait encore mieux si elle était gerer par une interruption d'un timer assez lent pour eviter le clignotement du detecteur. 

<img width="896" alt="Capture d’écran 2025-05-27 à 16 06 14" src="https://github.com/user-attachments/assets/eb322ae1-0d4b-45ef-a658-59852d14557b" />

## Ressource STM32 utilisées :

GPIO,
ADC,
DAC,
Ecran via la bibliothèque BSP_LCD,
DMA,
Timmer

