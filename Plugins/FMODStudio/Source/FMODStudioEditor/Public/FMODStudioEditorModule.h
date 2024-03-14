// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * The public interface to this module
 */
class IFMODStudioEditorModule : public IModuleInterface
{

public:
    /**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
    static inline IFMODStudioEditorModule &Get() { return FModuleManager::LoadModuleChecked<IFMODStudioEditorModule>("FMODStudioEditor"); }

    /**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
    static inline bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("FMODStudioEditor"); }

	/** This event is fired after all banks were reloaded */
	virtual FSimpleMulticastDelegate &BanksReloadedEvent() = 0;
};
