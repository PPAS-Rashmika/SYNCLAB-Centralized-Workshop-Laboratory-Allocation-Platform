// Database class එකට තිබිය යුතු functions ලැයිස්තුවයි. = 0 ඇති function වල වැඩ derived class එක ලියයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QList>
#include <QDate>
#include <QString>
#include <QStringList>
#include "studentrecord.h"

// UI එක database වර්ගය දැනගැනීම වෙනුවට මෙම පොදු functions භාවිත කරයි.
class IDatabaseManager
{
  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Base pointer එකෙන් delete කළත් derived database class එක නිසි ලෙස වසයි.
    virtual ~IDatabaseManager() {}

    // නව tables සාදයි; පැරණි database එකක් නම් දත්ත රැකගෙන නව tables වලට මාරු කරයි.
    virtual bool createTable() = 0;
    // නව booking එක insert කරයි; key එක සහ slots වෙනත් booking එකක භාවිතදැයිත් බලයි.
    virtual bool addRecord(const StudentRecord &record) = 0;
    // පවතින booking එක update කරයි. Key, created_at සහ admin_remarks මෙහි වෙනස් නොකරයි.
    virtual bool updateRecord(const StudentRecord &record) = 0;
    // ID එකෙන් booking එක මකා දමයි; එහි slot rows foreign key cascade මගින් මැකේ.
    virtual bool deleteRecord(int id) = 0;
    // Status එක පමණක් වෙනස් කරයි; Reject නොවන status එකකට යද්දී slots නැවත පරීක්ෂා කරයි.
    virtual bool updateRecordStatus(int id, const QString &status) = 0;
    // Booking එකේ admin remark එක පමණක් save කරයි.
    virtual bool updateAdminRemarks(int id, const QString &remarks) = 0;

    // Workshop එකක තෝරාගත් දිනයේ bookings අලුත්ම ID එක මුලින් ලබාදෙයි.
    virtual QList<StudentRecord> getRecordsByDateAndCategory(const QString &date,
                                                             const QString &category) = 0;
    // Reject නොකළ bookings ඇති දින ලබාදෙයි.
    virtual QList<QDate> getAllocatedDates(const QString &category) = 0;
    // ඉල්ලූ slots සියල්ල හිස් නම් true දෙයි; edit කරන booking එක excludeRecordId වලින් හැර දමයි.
    virtual bool isTimeSlotAvailable(const QString &date, const QString &category,
                                     const QStringList &requestedSlots,
                                     int excludeRecordId = -1) = 0;
    // එම workshop/date එකේ වෙන්කර ඇති slots ලබාදෙයි; Reject bookings ඉවත් කරයි.
    virtual QStringList getAllocatedTimeSlotsStringList(const QString &date, const QString &category,
                                                     int excludeRecordId = -1) = 0;
    // Workshop එකේ සියලු bookings අලුත් දිනය මුලින් ලැබෙන ලෙස ගනී.
    virtual QList<StudentRecord> getAllRecordsForCategory(const QString &category) = 0;
    // අවසන් database වැඩේ අසාර්ථක වූ හේතුව ලබාදෙයි.
    virtual QString getLastError() const = 0;
};

#endif
