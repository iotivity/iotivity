#ifndef IOTVT_SRM_SECURITY_INTERNALS_H
#define IOTVT_SRM_SECURITY_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

OicSecAcl_t* CBORPayloadToAcl(const uint8_t *payload, const size_t size);

void DeleteACLList(OicSecAcl_t* acl);

/**
 * This internal method is to retrieve the default ACL.
 * If SVR database in persistent storage got corrupted or
 * is not available for some reason, a default ACL is created
 * which allows user to initiate ACL provisioning again.
 */
OCStackResult  GetDefaultACL(OicSecAcl_t** defaultAcl);

/**
 * This internal method is the entity handler for ACL resources and
 * will handle REST request (GET/PUT/POST/DEL) for them.
 */
OCEntityHandlerResult ACLEntityHandler(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest * ehRequest, void* callbackParameter);

OCStackResult SetDefaultACL(OicSecAcl_t *acl);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_SECURITY_INTERNALS_H