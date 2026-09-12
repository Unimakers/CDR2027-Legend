#include "LUCKFOX.h"
#include "pamiboard.h"

// Le buffer stocke les bouts de mots pendant qu'ils arrivent
static String rx_buffer = "";

void luckfox_init(uint32_t baudrate) {
    // Configuration : Vitesse, Format (8 bits, pas de parité, 1 bit de stop), RX, TX
    Serial1.begin(baudrate, SERIAL_8N1, PAMI_UART1_RX, PAMI_UART1_TX);
    
    // On réserve un peu de RAM (256 octets) pour éviter la fragmentation de la mémoire
    rx_buffer.reserve(256); 
}

void luckfox_send(const String& message) {
    // envoie le message suivi d'un retour à la ligne (\r\n) 
    // pour que le script Python/C++ du Luckfox sache que c'est la fin de la commande.
    Serial1.println(message);
}

bool luckfox_receive(String& response) {
    while (Serial1.available()) {
        char c = Serial1.read();
        
        // le message est complet
        if (c == '\n') {
            response = rx_buffer;
            response.trim(); // Nettoie les espaces vides ou le \r invisible à la fin
            rx_buffer = "";  // On vide le buffer pour le prochain message
            
            // On ne renvoie vrai que si le message n'est pas une simple ligne vide
            if (response.length() > 0) {
                return true;
            }
        } 
        else {
            // Sinon, on ajoute la lettre au mot en cours
            rx_buffer += c;
        }
    }
    
    // Pas de message complet pour l'instant, le robot peut continuer sa vie
    return false; 
}