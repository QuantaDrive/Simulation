*03-03*

Er waren wat problemen met de naam van de nodes. Als de nodes gerendered werden kregen deze aparte namen. Het probleem
bleek uiteindelijk te liggen in de vector van ImGui (ImVector)

*09-03*

Ik had wat problemen met het linken van de nodes aan de daadwerkelijke instructies en de simulatiomanager die door iemand anders is gemaakt. Oplossing pending. 
Probleem met Id's bij nodes die sliders (input) gebruiken en daardoor conflicteerden. Oplossing. Assign ID en die ID weer poppen van een stack.

*TODO*

- Node met degrees -> globale var voor het opslaan van degrees in simManager (3 soorten)
- Absolute & relative zijn Booleans
- Rapid move and Linear move x,y,z
- Rapid move is zsm
- Linear is langzamer en heeft een slider nodig voor snelheid

currentPosition heeft de current degrees ook 

16-03 
- Moeite met het vinden van een manier om een visuele indicatie te geven van
- de positie waar de arm naartoe zou bewegen. 