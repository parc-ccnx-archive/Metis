/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * Tests the public API for the SystemInterfaces calls.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

// ==================================================================

LONGBOW_TEST_FIXTURE(PublicApi)
{
    LONGBOW_RUN_TEST_CASE(PublicApi, metisSystem_Interfaces);
    LONGBOW_RUN_TEST_CASE(PublicApi, metisSystem_GetMacAddressByName);
}

LONGBOW_TEST_FIXTURE_SETUP(PublicApi)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(PublicApi)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(PublicApi, metisSystem_Interfaces)
{
    MetisForwarder *metis = metisForwarder_Create(NULL);
    CPIInterfaceSet *set = metisSystem_Interfaces(metis);
    assertNotNull(set, "metisSystem_Interfaces return null set");

    size_t length = cpiInterfaceSet_Length(set);
    assertTrue(length > 0, "metisSystem_Interfaces returned no interfaces");

    // should verify that the list is correct, not just print it (case 829)
    for (size_t i = 0; i < length; i++) {
        CPIInterface *iface = cpiInterfaceSet_GetByOrdinalIndex(set, i);
        printf("Interface Index %u\n", cpiInterface_GetInterfaceIndex(iface));
        const CPIAddressList *list = cpiInterface_GetAddresses(iface);
        PARCJSONArray *jsonArray = cpiAddressList_ToJson(list);
        char *str = parcJSONArray_ToString(jsonArray);
        printf("%s\n", str);
        parcMemory_Deallocate((void **) &str);
        parcJSONArray_Release(&jsonArray);
    }

    cpiInterfaceSet_Destroy(&set);
    metisForwarder_Destroy(&metis);
}

// returns a strdup() of the interface name, use free(3)
char *
pickInterfaceName(MetisForwarder *metis)
{
    char *ifname = NULL;

    CPIInterfaceSet *set = metisSystem_Interfaces(metis);
    size_t length = cpiInterfaceSet_Length(set);
    assertTrue(length > 0, "metisSystem_Interfaces returned no interfaces");

    for (size_t i = 0; i < length; i++) {
        CPIInterface *iface = cpiInterfaceSet_GetByOrdinalIndex(set, i);
        const CPIAddressList *addressList = cpiInterface_GetAddresses(iface);

        size_t length = cpiAddressList_Length(addressList);
        for (size_t i = 0; i < length && !ifname; i++) {
            const CPIAddress *a = cpiAddressList_GetItem(addressList, i);
            if (cpiAddress_GetType(a) == cpiAddressType_LINK) {
                ifname = strdup(cpiInterface_GetName(iface));
            }
        }
    }

    cpiInterfaceSet_Destroy(&set);
    return ifname;
}

LONGBOW_TEST_CASE(PublicApi, metisSystem_GetMacAddressByName)
{
    MetisForwarder *metis = metisForwarder_Create(NULL);
    char *ifname = pickInterfaceName(metis);

    CPIAddress *a = metisSystem_GetMacAddressByName(metis, ifname);
    assertNotNull(a, "Got null mac address for interface %s", ifname);
    cpiAddress_Destroy(&a);
    metisForwarder_Destroy(&metis);
    free(ifname);
}

