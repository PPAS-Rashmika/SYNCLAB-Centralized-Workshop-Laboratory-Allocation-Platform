// SQLite සමඟ වැඩ කරන class එකයි. Query වල code එක databasemanager.cpp හි ඇත.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QSqlDatabase>
#include "idatabasemanager.h"

// IDatabaseManager හි functions SQLite සඳහා මෙහි ක්‍රියාත්මක කරයි.
class SqliteDatabaseManager : public IDatabaseManager
{
  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // SQLite connection එක සාදා foreign keys ක්‍රියාත්මක කර database file එක විවෘත කරයි.
    explicit SqliteDatabaseManager(const QString &dbName);
    // Database connection එක වසා Qt connection ලැයිස්තුවෙන් ඉවත් කරයි.
    ~SqliteDatabaseManager() override;


    // නව tables සාදයි; පැරණි database එකක් නම් දත්ත රැකගෙන නව tables වලට මාරු කරයි.
    bool createTable() override;

    // නව booking එක insert කරයි; key එක සහ slots වෙනත් booking එකක භාවිතදැයිත් බලයි.
    bool addRecord(const StudentRecord &record) override;
    // පවතින booking එක update කරයි. Key, created_at සහ admin_remarks මෙහි වෙනස් නොකරයි.
    bool updateRecord(const StudentRecord &record) override;
    // ID එකෙන් booking එක මකා දමයි; එහි slot rows foreign key cascade මගින් මැකේ.
    bool deleteRecord(int id) override;
    // Status එක පමණක් වෙනස් කරයි; Reject නොවන status එකකට යද්දී slots නැවත පරීක්ෂා කරයි.
    bool updateRecordStatus(int id, const QString &status) override;
    // Booking එකේ admin remark එක පමණක් save කරයි.
    bool updateAdminRemarks(int id, const QString &remarks) override;

    // Workshop එකක තෝරාගත් දිනයේ bookings අලුත්ම ID එක මුලින් ලබාදෙයි.
    QList<StudentRecord> getRecordsByDateAndCategory(const QString &date,
                                                     const QString &category) override;
    // Reject නොකළ bookings ඇති දින ලබාදෙයි.
    QList<QDate> getAllocatedDates(const QString &category) override;
    // ඉල්ලූ slots සියල්ල හිස් නම් true දෙයි; edit කරන booking එක excludeRecordId වලින් හැර දමයි.
    bool isTimeSlotAvailable(const QString &date, const QString &category,
                             const QStringList &requestedSlots, int excludeRecordId = -1) override;
    // එම workshop/date එකේ වෙන්කර ඇති slots ලබාදෙයි; Reject bookings ඉවත් කරයි.
    QStringList getAllocatedTimeSlotsStringList(const QString &date, const QString &category,
                                             int excludeRecordId = -1) override;
    // Workshop එකේ සියලු bookings අලුත් දිනය මුලින් ලැබෙන ලෙස ගනී.
    QList<StudentRecord> getAllRecordsForCategory(const QString &category) override;
    // අවසන් database වැඩේ අසාර්ථක වූ හේතුව ලබාදෙයි.
    QString getLastError() const override;

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    QSqlDatabase db; // විවෘත SQLite connection එක.
    QString connectionName; // Qt හි database connections වෙන් කර හඳුනාගන්නා නම.
    QString lastError; // අසාර්ථක වූ database වැඩේ පණිවිඩය.

    // Database එක විවෘතදැයි බලයි; නැතිනම් lastError සකසයි.
    bool ensureOpen();                                  
    // සම්බන්ධ save වැඩ එකම transaction එකකට අරඹයි; වෙනත් writes අවසන් වනතුරු SQLite බලා සිටියි.
    bool beginWrite();                                  
    // Transaction එක සම්පූර්ණයෙන් save කරයි; අසාර්ථක නම් rollback කරයි.
    bool commitWrite();                                 
    // Database වෙත යවන booking එකේ අනිවාර්ය දත්ත සහ email පරීක්ෂා කරයි.
    bool validateRecord(const StudentRecord &record);   
    // Workshop එක සහ student profile එක save කරයි; එකම index එකේ bookings එකම profile එක භාවිත කරයි.
    bool saveRelatedFacts(const StudentRecord &record);  
    // Booking එකේ පරණ slot rows මකා අලුත් list එකේ slot එකකට row එක බැගින් දමයි.
    bool replaceSlots(int recordId, const QStringList &slotList);  
};

#endif
