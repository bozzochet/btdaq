//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_DECODEDATAOCA_HH
#define DECODE_DECODEDATAOCA_HH

#include "DecodeData.hh"
#include "GenericEvent.hh"

class DecodeDataOCA : public DecodeData {
public:
    using EventOCA = GenericEvent<1, 24, 64, 5, 10, 0>;
    using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;
    using RHClassOCA = RHClass<EventOCA::GetNJINF(), EventOCA::GetNTDRS()>;

public:
    EventOCA *ev;
    RHClassOCA *rh;

    DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calnum);

    virtual ~DecodeDataOCA();

    virtual ::FlavorConfig FlavorConfig() final {
        return {EventOCA::GetNJINF(), EventOCA::GetNTDRS(), EventOCA::GetNCHAVA(), EventOCA::GetNADCS(),
                EventOCA::GetNVAS()};
    };

    virtual TString EventClassname() final { return ev->ClassName(); };

    bool ReadFileHeader(FILE *file, RHClassOCA* rhc);

    virtual int ReadOneEvent() final;

    virtual void ClearEvent() final { ev->Clear(); };

    // dummy for now
    virtual int SkipOneEvent(int evskip = 1) final { return 0; };

    virtual int GetTdrNum(size_t pos) final;

    virtual int GetTdrType(size_t pos) final;

private:
    std::string m_rawDir;
    std::string m_calDir;

    std::string m_filename;
    std::string m_calFilename;

    FILE *calfile = nullptr;
    calibOCA cals[EventOCA::GetNJINF() * EventOCA::GetNTDRS()]{};
    EventOCA::JArray<int> JinfMap{0};

    unsigned int m_numBoards = 12;//maximum
    unsigned int m_numBoardsFound = 0;//found during ReadOneEventFromFile

    void DumpRunHeader() override;

    void InitHistos();

    void OpenFile(const char *rawDir, const char *calDir, int runNum, int calNum) final;

    bool ProcessCalibration();

    int ReadOneEventFromFile(FILE *file, EventOCA *event);
};

#endif // DECODE_DECODEDATAOCA_HH
