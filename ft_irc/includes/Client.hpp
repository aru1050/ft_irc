/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:50:55 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 16:50:56 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    private:
        int         _fd;

        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _password;
        std::string _buffer;

        bool        _hasPass;
        bool        _hasNick;
        bool        _hasUser;
        bool        _isRegistered;

    public:
    Client(); // Constructeur par défaut
    ~Client(); // Destructeur
    Client(int fd); // Constructeur avec file descriptor

    // Retourne le file descriptor du client
    int getFd() const;

    // Modifie le file descriptor du client
    void setFd(int fd);

    // Retourne le nickname du client
    std::string getNickname() const;

    // Modifie le nickname du client
    void setNickname(const std::string& nickname);

    // Retourne le username du client
    std::string getUsername() const;

    // Modifie le username du client
    void setUsername(const std::string& username);

    // Retourne le realname du client
    std::string getRealname() const;

    // Modifie le realname du client
    void setRealname(const std::string& realname);

    // Retourne le mot de passe du client
    std::string getPassword() const;

    // Modifie le mot de passe du client
    void setPassword(const std::string& password);

    // Retourne le contenu actuel du buffer
    std::string getBuffer() const;

    // Remplace complètement le contenu du buffer
    void setBuffer(const std::string& buffer);

    // Ajoute les données reçues par recv() au buffer afin de
    // reconstruire une commande IRC complète pouvant arriver
    // en plusieurs paquets réseau.
    void appendBuffer(const std::string& data);

    // Vide le buffer après traitement d'une ou plusieurs commandes IRC
    void clearBuffer();

    // Vérifie si la commande PASS a déjà été reçue
    bool hasPass() const;

    // Indique que la commande PASS a été reçue ou non
    void setHasPass(bool value);

    // Vérifie si la commande NICK a déjà été reçue
    bool hasNick() const;

    // Indique que la commande NICK a été reçue ou non
    void setHasNick(bool value);

    // Vérifie si la commande USER a déjà été reçue
    bool hasUser() const;

    // Indique que la commande USER a été reçue ou non
    void setHasUser(bool value);

    // Vérifie si le client est officiellement enregistré
    bool isRegistered() const;

    // Définit l'état d'enregistrement du client
    void setRegistered(bool value);

    // Vérifie si PASS, NICK et USER ont été reçus
    // et si le client peut être enregistré
    bool isReadyToRegister() const;
};

#endif