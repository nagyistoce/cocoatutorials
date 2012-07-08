#include <ole2.h>
int main()
{
    OleInitialize(NULL) ;
    GUID          guid  ;
    CoCreateGuid(&guid) ;

}


#if 0
void GetFormattedGuid(CString& rString)
{
        // load appropriate formatting string
        CString strFormat;
        strFormat.LoadString(IDS_FORMATS+m_nGuidType);

        // then format into destination
        rString.Format(strFormat,
                // first copy...
                m_guid.Data1, m_guid.Data2, m_guid.Data3,
                m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3],
                m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7],
                // second copy...
                m_guid.Data1, m_guid.Data2, m_guid.Data3,
                m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3],
                m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);
}
#endif
