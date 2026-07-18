#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Channel
{
	private:
		// Nom du channel : #42, #general, etc.
		std::string _name;

		// Sujet du channel
		std::string _topic;

		// Liste des clients présents dans le channel
		std::vector<int> _clients;

		// Liste des opérateurs du channel
		std::vector<int> _operators;

		// Liste des clients invités pour le mode +i
		std::vector<int> _invitedClients;

		// Mode +i : channel accessible uniquement sur invitation
		bool _inviteOnly;

		// Mode +t : seul un opérateur peut changer le topic
		bool _topicRestricted;

		// Mode +k : mot de passe obligatoire
		bool _hasPassword;
		std::string _password;

		// Mode +l : limite d'utilisateurs
		bool _hasUserLimit;
		int _userLimit;

	public:
		Channel();
		Channel(std::string name);
		~Channel();

		std::string getName() const;
		std::string getTopic() const;
		std::string getPassword() const;
		int getUserLimit() const;
		int getClientCount() const;

		void setTopic(std::string topic);

		void addClient(int clientFd);
		void removeClient(int clientFd);
		bool hasClient(int clientFd) const;

		void addOperator(int clientFd);
		void removeOperator(int clientFd);
		bool isOperator(int clientFd) const;

		void inviteClient(int clientFd);
		void removeInvite(int clientFd);
		bool isInvited(int clientFd) const;

		void setInviteOnly(bool value);
		bool isInviteOnly() const;

		void setTopicRestricted(bool value);
		bool isTopicRestricted() const;

		void setPassword(std::string password);
		void removePassword();
		bool hasPassword() const;

		void setUserLimit(int limit);
		void removeUserLimit();
		bool hasUserLimit() const;

		// Vérifie si un client a le droit de rejoindre le channel
		bool canJoin(int clientFd, std::string password) const;
};

#endif