import os
import select
import py_compile
import base64

ota_file = None 
target_filename = ""
target_tmp_path = ""

os.system("stty -F /dev/ttyS3 115200 raw -echo -echoe -echok -icrnl")

def process_message(msg, fd):
    global ota_file, target_filename, target_tmp_path
    
    msg = msg.strip()
    print(f"[RECU] {repr(msg)}")
    
    if ":" in msg:
        name, data = msg.split(":", 1)
    else:
        name = msg
        data = ""
        
    if name == "OTA_START":
        target_filename = data if data else "uart.py"
        target_filename = os.path.basename(target_filename)
        
        # --- VERROUILLAGE DE SECURITE : UART.PY EST IMMUABLE ---
        if target_filename == "uart.py":
            print(" -> [REFUS] uart.py est protege et immuable ! Modification interdite.")
            os.write(fd, b"<OTA_STATUS:ERROR_PROTECTED>\n")
            ota_file = None
            return
        
        target_tmp_path = f"/tmp/{target_filename}"
        print(f" -> DEBUT OTA POUR LE FICHIER : {target_filename}")
        ota_file = open(target_tmp_path, "w")
        
    elif name == "OTA_LINE":
        if ota_file is not None:
            try:
                decoded_line = base64.b64decode(data).decode('utf-8')
                ota_file.write(decoded_line.replace('\r', '') + "\n")
            except Exception as e:
                print(" -> Erreur de decodage Base64 :", e)
                
    elif name == "OTA_END":
        print(f" -> FIN OTA. VERIFICATION DE {target_filename}...")
        if ota_file is not None:
            ota_file.close()
            ota_file = None
            
            try:
                py_compile.compile(target_tmp_path, doraise=True)
                print(" -> Code valide ! Installation et redemarrage...")
                os.write(fd, b"<OTA_STATUS:SUCCESS>\n")
                
                final_path = f"/root/scripts_python/{target_filename}"
                os.system(f"mv {target_tmp_path} {final_path}")
                os.system(f"sed -i 's/\\r//g' {final_path}")
                
                # RedÃ©marrage pour que Python prenne en compte les nouveaux scripts/imports
                os.system("/sbin/reboot -f")
                
            except py_compile.PyCompileError as e:
                print(" -> ERREUR DE SYNTAXE !")
                os.write(fd, b"<OTA_STATUS:ERROR_SYNTAX>\n")
                if os.path.exists(target_tmp_path):
                    os.remove(target_tmp_path)
                    
    elif name == "FILE_LIST":
        print(" -> Demande de la liste des fichiers")
        try:
            current_dir = "/root/scripts_python"
            files = [f for f in os.listdir(current_dir) if os.path.isfile(os.path.join(current_dir, f))]
            files_str = ",".join(files)
            
            reponse = f"<FILE_LIST_RESP:{files_str}>\n"
            os.write(fd, reponse.encode('utf-8'))
            print(f"[ENVOYE] {reponse.strip()}")
            return 
            
        except Exception as e:
            print(" -> Erreur listage fichiers :", e)

    elif name == "FILE_DEL":
        filename = os.path.basename(data.strip())
        print(f" -> Demande de suppression du fichier : {filename}")
        
        if filename == "uart.py":
            print(" -> [REFUS] Impossible de supprimer le fichier principal !")
            os.write(fd, b"<FILE_STATUS:ERR_PROTECTED>\n")
        else:
            file_path = f"/root/scripts_python/{filename}"
            if os.path.exists(file_path):
                os.remove(file_path)
                print(" -> Fichier supprime avec succes.")
                os.write(fd, b"<FILE_STATUS:SUCCESS>\n")
            else:
                os.write(fd, b"<FILE_STATUS:NOT_FOUND>\n")
        return

    elif name == "FILE_REN":
        try:
            old_name, new_name = data.split("|", 1)
            old_name = os.path.basename(old_name.strip())
            new_name = os.path.basename(new_name.strip())
            
            print(f" -> Renommage de {old_name} vers {new_name}")
            
            if old_name == "uart.py" or new_name == "uart.py":
                os.write(fd, b"<FILE_STATUS:ERR_PROTECTED>\n")
            else:
                old_path = f"/root/scripts_python/{old_name}"
                new_path = f"/root/scripts_python/{new_name}"
                
                if os.path.exists(old_path):
                    os.rename(old_path, new_path)
                    os.write(fd, b"<FILE_STATUS:SUCCESS>\n")
                else:
                    os.write(fd, b"<FILE_STATUS:NOT_FOUND>\n")
        except Exception as e:
            print(" -> Erreur de format pour FILE_REN :", e)
        return

    elif name == "PING":
        print(" -> Le robot verifie la connexion.")
        reponse = f"<ACK:{name}>\n"
        os.write(fd, reponse.encode('utf-8'))
        
    else:
        # --- REDIRECTION AUTOMATIQUE VERS STRATEGY.PY ---
        reponse_strategy = None
        try:
            import strategy
            if hasattr(strategy, 'gerer_message'):
                reponse_strategy = strategy.gerer_message(name, data)
        except ImportError:
            print(f" -> [ATTENTION] Commande inconnue '{name}' et aucun fichier strategy.py trouve.")
        except Exception as e:
            print(f" -> Erreur d'execution dans strategy.py : {e}")
            
        if reponse_strategy:
            if not reponse_strategy.endswith("\n"):
                reponse_strategy += "\n"
            os.write(fd, reponse_strategy.encode('utf-8'))
            print(f"[ENVOYE (Strategy)] {reponse_strategy.strip()}")
        else:
            reponse = f"<ACK:{name}>\n"
            os.write(fd, reponse.encode('utf-8'))

def main():
    fd = os.open("/dev/ttyS3", os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    
    msg_ready = "<READY>\n"
    os.write(fd, msg_ready.encode('utf-8'))
    
    buffer = ""
    in_message = False
    print("Luckfox pret (Noyau de communication). En attente de l'ESP32...")
    
    try:
        while True:
            r, w, e = select.select([fd], [], [], 0.01)
            if fd in r:
                try:
                    raw_data = os.read(fd, 1024)
                    text_data = raw_data.decode('utf-8', errors='ignore')
                    
                    for char in text_data:
                        if char == '<':
                            buffer = ""
                            in_message = True
                        elif char == '>' and in_message:
                            in_message = False
                            process_message(buffer, fd)
                        elif in_message:
                            buffer += char
                except OSError:
                    pass
    except KeyboardInterrupt:
        print("\nFermeture du port serie.")
        os.close(fd)

if __name__ == "__main__":
    main()