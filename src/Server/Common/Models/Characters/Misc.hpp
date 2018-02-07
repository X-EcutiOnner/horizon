//
// Created by SagunKho on 01/02/2018.
//

#ifndef HORIZON_MODELS_CHARACTERS_MISCDATA_HPP
#define HORIZON_MODELS_CHARACTERS_MISCDATA_HPP

#include "Server/Common/Horizon.hpp"
#include "Server/Common/Server.hpp"
#include "Core/Database/MySqlConnection.hpp"

namespace Horizon
{
namespace Models
{
namespace Characters
{
class Misc
{
public:
	Misc() {}
	~Misc() {}

	/**
	 * Load all fields from the database into this instance.
	 * @param server
	 * @param char_id
	 * @return
	 */
	bool LoadFromDatabase(Server *server, uint32_t char_id)
	{
		std::string query = "SELECT * FROM character_misc_data WHERE char_id = ?";
		auto sql = server->MySQLBorrow();
		bool ret = false;

		try {
			sql::PreparedStatement *pstmt = sql->getConnection()->prepareStatement(query);
			pstmt->setInt(1, char_id);
			sql::ResultSet *res = pstmt->executeQuery();

			if (res != nullptr && res->next()) {
				/**
				 * Create Game Account Data
				 */
				setCharacterID(char_id);
				setRenameCount((uint8_t) res->getUInt("rename_count"));
				setUniqueItemCounter(res->getUInt("unique_item_counter"));
				setHotkeyRowIndex((uint16_t) res->getUInt("hotkey_row_index"));
				setChangeSlotCount((uint8_t) res->getUInt("change_slot"));
				ret = true;
			}

			delete res;
			delete pstmt;
		} catch (sql::SQLException &e) {
			DBLog->error("Models::Characters::Misc::LoadFromDatabase: {}", e.what());
		}

		server->MySQLUnborrow(sql);

		return ret;
	}

	/**
	 * @brief Save this model to the database in its current state.
	 * @param[in|out] server   instance of the server object used to borrow mysql connections.
	 */
	void save(Server *server)
	{
		auto sql = server->MySQLBorrow();

		std::string query = "REPLACE INTO `character_misc_data` "
			"(`id`, `rename_count`, `unique_item_counter`, `hotkey_row_index`, `change_slot_count`) "
			"VALUES (?, ?, ?, ?, ?);";

		try {
			sql::PreparedStatement *pstmt = sql->getConnection()->prepareStatement(query);
			pstmt->setInt(1, getCharacterID());
			pstmt->setInt(2, getRenameCount());
			pstmt->setInt64(3, getUniqueItemCounter());
			pstmt->setInt(4, getHotkeyRowIndex());
			pstmt->setInt(5, getChangeSlotCount());
			pstmt->executeUpdate();
			delete pstmt;
		} catch (sql::SQLException &e) {
			DBLog->error("SQLException: {}", e.what());
		}

		server->MySQLUnborrow(sql);
	}

	/* Character ID */
	uint32_t getCharacterID() const { return character_id; }
	void setCharacterID(uint32_t character_id) { Misc::character_id = character_id; }
	/* Character Rename Count */
	uint8_t getRenameCount() const { return rename_count; }
	void setRenameCount(uint8_t rename_count) { Misc::rename_count = rename_count; }
	/* Unique Item Counter */
	uint64_t getUniqueItemCounter() const { return unique_item_counter; }
	void setUniqueItemCounter(uint64_t unique_item_counter) { Misc::unique_item_counter = unique_item_counter; }
	/* Hotkey Row Index */
	uint8_t getHotkeyRowIndex() const { return hotkey_row_index; }
	void setHotkeyRowIndex(uint8_t hotkey_row_index) { Misc::hotkey_row_index = hotkey_row_index; }
	/* Change Slot Count */
	uint8_t getChangeSlotCount() const { return change_slot_count; }
	void setChangeSlotCount(uint8_t change_slot_count) { Misc::change_slot_count = change_slot_count; }
private:
	uint32_t character_id{0};
	uint8_t rename_count{0};
	uint64_t unique_item_counter{0};
	uint8_t hotkey_row_index{0};
	uint8_t change_slot_count{0};
};
}
}
}
#endif // HORIZON_MODELS_CHARACTERS_MISCDATA_HPP
